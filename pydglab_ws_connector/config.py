from pydantic_settings import BaseSettings
from pydantic import Field
import os
import sys


def get_base_dir():
    """获取当前执行文件所在目录"""
    if getattr(sys, 'frozen', False):
        # 如果是打包后的可执行文件
        return os.path.dirname(sys.executable)
    else:
        # 如果是Python脚本
        return os.path.dirname(os.path.abspath(__file__))


class Settings(BaseSettings):
    """DGLab WebSocket服务器配置"""
    ws_host: str = Field(default="0.0.0.0", description="WebSocket服务器监听主机")
    ws_port: int = Field(default=5678, description="WebSocket服务器监听端口")
    ws_external_url: str = Field(
        default="ws://127.0.0.1:5678",
        description="用于生成二维码的外部WebSocket URL"
    )
    connector_ws_host: str = Field(default="0.0.0.0", description="Connector WebSocket服务器监听主机")
    connector_ws_port: int = Field(default=5679, description="Connector WebSocket服务器监听端口")

    class Config:
        env_file = os.path.join(get_base_dir(), ".env")
        env_file_encoding = "utf-8"
        case_sensitive = False


settings = Settings() 