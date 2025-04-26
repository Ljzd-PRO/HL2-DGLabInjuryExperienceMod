from pydantic_settings import BaseSettings
from pydantic import Field


class Settings(BaseSettings):
    """DGLab WebSocket服务器配置"""
    ws_host: str = Field(default="0.0.0.0", description="WebSocket服务器监听主机")
    ws_port: int = Field(default=5678, description="WebSocket服务器监听端口")
    ws_external_url: str = Field(
        default="ws://192.168.1.161:5678",
        description="用于生成二维码的外部WebSocket URL"
    )
    connector_ws_host: str = Field(default="0.0.0.0", description="Connector WebSocket服务器监听主机")
    connector_ws_port: int = Field(default=5679, description="Connector WebSocket服务器监听端口")

    class Config:
        env_file = ".env"
        env_file_encoding = "utf-8"
        case_sensitive = False


settings = Settings() 