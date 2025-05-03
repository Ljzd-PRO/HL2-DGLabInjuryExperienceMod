import asyncio
import qrcode
from pydglab_ws import DGLabWSServer
from pydglab_ws_connector.config import settings
from pydglab_ws_connector.connector_server import start_connector_server, set_dglab_client


def print_qrcode(url: str):
    """在终端打印二维码"""
    qr = qrcode.QRCode()
    qr.add_data(url)
    qr.print_ascii()


async def main():
    """主函数"""
    # 启动Connector服务器
    connector_task = asyncio.create_task(start_connector_server())
    
    async with DGLabWSServer(settings.ws_host, settings.ws_port, 60) as server:
        # 创建本地客户端
        client = server.new_local_client()
        
        # 设置DGLab客户端实例
        set_dglab_client(client)

        # 获取并显示二维码
        url = client.get_qrcode(settings.ws_external_url)
        print("请用 DG-Lab App 扫描二维码以连接")
        print_qrcode(url)

        # 等待绑定
        await client.bind()
        print(f"已与 App {client.target_id} 成功绑定")

        # 保持服务器运行
        try:
            await asyncio.gather(connector_task)
        except asyncio.CancelledError:
            pass


if __name__ == "__main__":
    asyncio.run(main()) 