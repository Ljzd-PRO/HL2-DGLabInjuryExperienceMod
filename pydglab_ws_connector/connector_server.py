import asyncio
import json
import logging
from typing import Optional
from websockets.frames import CloseCode
from websockets.server import WebSocketServerProtocol, serve
from pydglab_ws import Channel, StrengthOperationType, DGLabLocalClient
from pydglab_ws_connector.config import settings

# 配置日志
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

# 存储DGLab客户端实例
dglab_client: Optional[DGLabLocalClient] = None

# 存储GUI实例
gui_instance = None

def set_gui_instance(gui):
    """设置GUI实例"""
    global gui_instance
    gui_instance = gui

async def handle_websocket(websocket: WebSocketServerProtocol):
    """处理WebSocket连接"""
    global dglab_client
    if not dglab_client:
        logger.error("DGLab客户端未初始化")
        await websocket.close(CloseCode.TRY_AGAIN_LATER, "DGLab客户端未初始化")
        return

    logger.info(f"新的外部客户端连接: {websocket.remote_address}")
    
    try:
        async for message in websocket:
            try:
                data = json.loads(message)
                method = data.get("method")
                params = data.get("params", {})
                
                if method == "set_strength":
                    channel = Channel[params.get("channel")]
                    operation_type = StrengthOperationType[params.get("operation_type")]
                    value = params.get("value")
                    await dglab_client.set_strength(channel, operation_type, value)
                    logger.info(f"执行set_strength: channel={channel}, operation_type={operation_type}, value={value}")
                    
                    # 更新GUI界面
                    if gui_instance:
                        if channel == Channel.A:
                            gui_instance.root.after(0, lambda: gui_instance.channel_a_scale.set(value))
                        else:
                            gui_instance.root.after(0, lambda: gui_instance.channel_b_scale.set(value))
                
                elif method == "add_pulses":
                    channel = Channel[params.get("channel")]
                    pulses = params.get("pulses", [])
                    await dglab_client.add_pulses(channel, *pulses)
                    logger.info(f"执行add_pulses: channel={channel}, pulses={pulses}")
                
                elif method == "clear_pulses":
                    channel = Channel[params.get("channel")]
                    await dglab_client.clear_pulses(channel)
                    logger.info(f"执行clear_pulses: channel={channel}")
                
                else:
                    logger.warning(f"未知的方法: {method}")
                    await websocket.send(json.dumps({"error": "未知的方法"}))
            
            except json.JSONDecodeError:
                logger.error("无效的JSON格式")
                await websocket.send(json.dumps({"error": "无效的JSON格式"}))
            except KeyError as e:
                logger.error(f"参数错误: {str(e)}")
                await websocket.send(json.dumps({"error": f"参数错误: {str(e)}"}))
            except Exception as e:
                logger.error(f"执行命令时出错: {str(e)}")
                await websocket.send(json.dumps({"error": f"执行命令时出错: {str(e)}"}))
    
    except Exception as e:
        logger.error(f"WebSocket连接错误: {str(e)}")
    finally:
        logger.info(f"外部客户端断开连接: {websocket.remote_address}")

async def start_connector_server():
    """启动Connector WebSocket服务器"""
    server = await serve(
        handle_websocket,
        settings.connector_ws_host,
        settings.connector_ws_port
    )
    
    logger.info(f"Connector WebSocket服务器启动在 ws://{settings.connector_ws_host}:{settings.connector_ws_port}")
    
    await asyncio.Future()  # 保持服务器运行

def set_dglab_client(client: object):
    """设置DGLab客户端实例"""
    global dglab_client
    dglab_client = client 