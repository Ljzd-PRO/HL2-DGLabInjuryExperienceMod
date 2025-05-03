import tkinter as tk
from tkinter import ttk, messagebox, scrolledtext
import asyncio
import threading
import qrcode
from PIL import ImageTk
import netifaces
import logging
import webbrowser
from pydglab_ws import DGLabWSServer, Channel, StrengthOperationType
from config import settings
from connector_server import start_connector_server, set_dglab_client, set_gui_instance

class DGLabGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("PyDGLab-WS 连接器")
        self.root.geometry("650x650")
        
        # 创建主框架
        self.main_frame = ttk.Frame(root, padding="10")
        self.main_frame.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
        
        # 网络接口选择
        self.interface_frame = ttk.Frame(self.main_frame)
        self.interface_frame.grid(row=0, column=0, columnspan=2, pady=10, sticky=(tk.W, tk.E))
        ttk.Label(self.interface_frame, text="网络接口:").pack(side=tk.LEFT, padx=5)
        self.interface_var = tk.StringVar()
        self.interface_combo = ttk.Combobox(self.interface_frame, textvariable=self.interface_var, state="readonly")
        self.interface_combo.pack(side=tk.LEFT, padx=5, fill=tk.X, expand=True)
        self.interface_combo.bind("<<ComboboxSelected>>", self.on_interface_change)
        
        # 状态标签
        self.status_label = ttk.Label(self.main_frame, text="未连接")
        self.status_label.grid(row=1, column=0, columnspan=2, pady=10)
        
        # 二维码显示区域
        self.qr_frame = ttk.Frame(self.main_frame)
        self.qr_frame.grid(row=2, column=0, columnspan=2, pady=10)
        self.qr_label = ttk.Label(self.qr_frame)
        self.qr_label.pack()
        
        # 日志输出区域
        self.log_frame = ttk.LabelFrame(self.main_frame, text="日志", padding="10")
        self.log_frame.grid(row=3, column=0, columnspan=2, pady=10, sticky=(tk.W, tk.E, tk.N, tk.S))
        
        self.log_text = scrolledtext.ScrolledText(self.log_frame, wrap=tk.WORD, width=80, height=15)
        self.log_text.pack(fill=tk.BOTH, expand=True)
        self.log_text.config(state=tk.DISABLED)
        
        # 底部信息区域
        self.bottom_frame = ttk.Frame(self.main_frame)
        self.bottom_frame.grid(row=4, column=0, columnspan=2, pady=5)
        
        # 版权信息
        copyright_text = "Copyright © 2025 Ljzd-PRO. 版本 1.0.0"
        ttk.Label(self.bottom_frame, text=copyright_text, font=("Arial", 8)).pack()
        
        # GitHub链接
        github_url = "https://github.com/Ljzd-PRO/HL2-DGLabInjuryExperienceMod"
        github_label = ttk.Label(
            self.bottom_frame, 
            text="GitHub 项目主页", 
            font=("Arial", 8, "underline"),
            foreground="blue",
            cursor="hand2"
        )
        github_label.pack()
        github_label.bind("<Button-1>", lambda e: webbrowser.open(github_url))
        
        # 初始化变量
        self.dglab_client = None
        self.server_task = None
        self.connector_task = None
        self.interface_map = {}  # 存储接口名称到接口标识的映射
        
        # 设置GUI实例
        set_gui_instance(self)
        
        # 配置日志处理器
        self.setup_logging()
        
        # 初始化网络接口列表
        self.init_network_interfaces()
        
        # 启动服务器
        self.start_server()
    
    def setup_logging(self):
        """配置日志处理器"""
        class TextHandler(logging.Handler):
            def __init__(self, text_widget):
                logging.Handler.__init__(self)
                self.text_widget = text_widget
                self.text_widget.config(state=tk.NORMAL)
                self.text_widget.delete(1.0, tk.END)
                self.text_widget.config(state=tk.DISABLED)
            
            def emit(self, record):
                msg = self.format(record)
                def append():
                    self.text_widget.config(state=tk.NORMAL)
                    self.text_widget.insert(tk.END, msg + '\n')
                    self.text_widget.see(tk.END)
                    self.text_widget.config(state=tk.DISABLED)
                self.text_widget.after(0, append)
        
        # 创建日志处理器
        text_handler = TextHandler(self.log_text)
        text_handler.setFormatter(logging.Formatter('%(asctime)s - %(levelname)s - %(message)s'))
        
        # 配置根日志记录器
        root_logger = logging.getLogger()
        root_logger.setLevel(logging.INFO)
        root_logger.addHandler(text_handler)
    
    def get_interface_name(self, interface):
        """获取网络接口的友好名称"""
        try:
            # 获取接口的IP地址
            addrs = netifaces.ifaddresses(interface)
            if netifaces.AF_INET in addrs:
                for addr in addrs[netifaces.AF_INET]:
                    if 'addr' in addr and not addr['addr'].startswith('127.'):
                        ip = addr['addr']
                        
                        # 尝试获取接口描述
                        try:
                            import winreg
                            key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, 
                                               f"SYSTEM\\CurrentControlSet\\Control\\Network\\{{4D36E972-E325-11CE-BFC1-08002BE10318}}\\{interface}\\Connection")
                            desc = winreg.QueryValueEx(key, "Name")[0]
                            return f"{desc} ({ip})"
                        except:
                            # 如果无法获取描述，则使用IP地址
                            return f"网络接口 ({ip})"
        except:
            pass
        return interface
    
    def init_network_interfaces(self):
        """初始化网络接口列表"""
        interfaces = []
        for interface in netifaces.interfaces():
            try:
                addrs = netifaces.ifaddresses(interface)
                if netifaces.AF_INET in addrs:
                    for addr in addrs[netifaces.AF_INET]:
                        if 'addr' in addr and not addr['addr'].startswith('127.'):
                            # 获取友好名称
                            friendly_name = self.get_interface_name(interface)
                            interfaces.append(friendly_name)
                            # 存储映射关系
                            self.interface_map[friendly_name] = interface
                            break
            except:
                continue
        
        if interfaces:
            self.interface_combo['values'] = interfaces
            self.interface_combo.set(interfaces[0])
            self.on_interface_change(None)
            logging.info(f"找到 {len(interfaces)} 个可用的网络接口")
        else:
            messagebox.showerror("错误", "未找到可用的网络接口")
            logging.error("未找到可用的网络接口")
    
    def on_interface_change(self, event):
        """网络接口变化回调"""
        if self.dglab_client:
            friendly_name = self.interface_var.get()
            interface = self.interface_map.get(friendly_name)
            if not interface:
                return
                
            try:
                addrs = netifaces.ifaddresses(interface)
                if netifaces.AF_INET in addrs:
                    for addr in addrs[netifaces.AF_INET]:
                        if 'addr' in addr and not addr['addr'].startswith('127.'):
                            ip = addr['addr']
                            url = self.dglab_client.get_qrcode(f"ws://{ip}:{settings.ws_port}")
                            self.show_qrcode(url)
                            logging.info(f"已更新二维码，使用网络接口: {friendly_name}")
                            break
            except Exception as e:
                messagebox.showerror("错误", f"获取网络接口地址失败: {str(e)}")
                logging.error(f"获取网络接口地址失败: {str(e)}")
    
    def start_server(self):
        """启动服务器线程"""
        def run_server():
            asyncio.run(self._start_server())
        
        server_thread = threading.Thread(target=run_server, daemon=True)
        server_thread.start()
        logging.info("服务器线程已启动")
    
    async def _start_server(self):
        """启动服务器"""
        # 启动Connector服务器
        self.connector_task = asyncio.create_task(start_connector_server())
        logging.info("Connector服务器已启动")
        
        async with DGLabWSServer(settings.ws_host, settings.ws_port, 60) as server:
            # 创建本地客户端
            client = server.new_local_client()
            set_dglab_client(client)
            
            # 更新UI状态
            self.root.after(0, lambda: self.status_label.config(text="等待连接..."))
            logging.info("等待DG-Lab App连接...")
            
            # 生成并显示二维码
            self.dglab_client = client
            self.on_interface_change(None)
            
            # 等待绑定
            await client.bind()
            
            # 更新UI状态
            self.root.after(0, lambda: self.status_label.config(text=f"已连接: {client.target_id}"))
            logging.info(f"已与DG-Lab App {client.target_id} 成功连接")
            
            # 保持服务器运行
            try:
                await asyncio.gather(self.connector_task)
            except asyncio.CancelledError:
                pass
    
    def show_qrcode(self, url):
        """显示二维码"""
        qr = qrcode.QRCode()
        qr.add_data(url)
        qr.make()
        
        # 创建二维码图像
        img = qr.make_image(fill_color="black", back_color="white")
        img = img.resize((200, 200))
        
        # 转换为Tkinter可用的格式
        photo = ImageTk.PhotoImage(img)
        self.qr_label.config(image=photo)
        self.qr_label.image = photo
        logging.info(f"已生成二维码，URL: {url}")

def main():
    root = tk.Tk()
    app = DGLabGUI(root)
    root.mainloop()

if __name__ == "__main__":
    main() 