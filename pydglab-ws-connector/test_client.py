import asyncio
import json
import logging
from websockets.client import connect

# 配置日志
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)


async def test_set_strength(websocket):
    """测试设置强度"""
    # 测试A通道设置强度
    await websocket.send(json.dumps({
        "method": "set_strength",
        "params": {
            "channel": "A",
            "operation_type": "SET_TO",
            "value": 100
        }
    }))
    logger.info("发送set_strength命令: A通道设置为100")

    # 测试B通道增加强度
    await websocket.send(json.dumps({
        "method": "set_strength",
        "params": {
            "channel": "B",
            "operation_type": "INCREASE",
            "value": 20
        }
    }))
    logger.info("发送set_strength命令: B通道增加20")


async def test_add_pulses(websocket):
    """测试添加波形"""
    # 测试A通道添加波形
    await websocket.send(json.dumps({
        "method": "add_pulses",
        "params": {
            "channel": "A",
            "pulses": [
                ((10, 10, 20, 30), (0, 5, 10, 50)),
                ((10, 10, 20, 30), (0, 5, 10, 50))
            ]
        }
    }))
    logger.info("发送add_pulses命令: A通道添加波形")


async def test_clear_pulses(websocket):
    """测试清空波形"""
    # 测试清空A通道波形
    await websocket.send(json.dumps({
        "method": "clear_pulses",
        "params": {
            "channel": "A"
        }
    }))
    logger.info("发送clear_pulses命令: 清空A通道波形")


async def main():
    """主函数"""
    try:
        async with connect("ws://localhost:5679") as websocket:
            logger.info("已连接到Connector服务器")

            # 测试设置强度
            await test_set_strength(websocket)
            await asyncio.sleep(2)  # 等待2秒

            # 测试添加波形
            await test_add_pulses(websocket)
            await asyncio.sleep(2)  # 等待2秒

            # 测试清空波形
            await test_clear_pulses(websocket)
            await asyncio.sleep(2)  # 等待2秒

            # 测试错误情况
            # 1. 未知方法
            await websocket.send(json.dumps({
                "method": "unknown_method",
                "params": {}
            }))
            logger.info("发送未知方法测试")

            # 2. 无效的JSON
            await websocket.send("invalid json")
            logger.info("发送无效JSON测试")

            # 3. 参数错误
            await websocket.send(json.dumps({
                "method": "set_strength",
                "params": {
                    "channel": "INVALID",
                    "operation_type": "SET_TO",
                    "value": 100
                }
            }))
            logger.info("发送参数错误测试")

            # 等待响应
            while True:
                try:
                    response = await websocket.recv()
                    logger.info(f"收到响应: {response}")
                except Exception as e:
                    logger.error(f"接收响应时出错: {str(e)}")
                    break

    except Exception as e:
        logger.error(f"连接错误: {str(e)}")


if __name__ == "__main__":
    asyncio.run(main()) 