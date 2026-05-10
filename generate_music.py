import requests
import json
import os

API_KEY = "sk-cp-4uDL2KEWaNHDCxSxNndecOIAJ2xHbdFuUegAk24UTWpV_zkCNMYpjGIilqgRpFUg1oBcer6iEtoH8kbAg9euOSlVh04FJ9GqagSx6y5R6MN2GtAw04ulX_0"

LYRICS = """蔚蓝的深处 藏着古老的歌
珊瑚礁彩色的梦 鱼群穿梭
多少年岁月 静静流淌
如今却听见 她在轻声叹

塑料漂浮 油污蔓延
海面之下 沉默的悲鸣
那些曾自由的生灵
是否还能等到 澄清

还海洋一片蔚蓝
让浪花重新洁白如雪
还海洋一片清澈
让生命继续欢歌
每一次选择 都是力量
汇聚你我 守护这片蔚蓝
海洋在呼唤 你我听见

潮汐冲刷着 搁浅的垃圾
海龟误食着 人类的废弃物
塑料袋飘摇 像水母的泪
海岸线不再洁净 只有疲惫

从你我开始 不再沉默
减少塑料 垃圾分类
选择可持续 绿色生活
让海洋重新呼吸

还海洋一片蔚蓝
让浪花重新洁白如雪
还海洋一片清澈
让生命继续欢歌
每一次选择 都是力量
汇聚你我 守护这片蔚蓝
海洋在呼唤 你我听见

一滴水的力量 能汇成海洋
一个人的行动 能改变方向
我们共同的家园
值得被温柔以待

还海洋一片蔚蓝
让浪花重新洁白如雪
还海洋一片清澈
让生命继续欢歌
每一次选择 都是力量
汇聚你我 守护这片蔚蓝
海洋在呼唤 你我听见"""

def generate_music():
    url = "https://api.minimax.io/music/generation"

    headers = {
        "Authorization": f"Bearer {API_KEY}",
        "Content-Type": "application/json"
    }

    payload = {
        "model": "music-2.6",
        "lyrics": LYRICS,
        "title": "海洋的呼唤",
        "genre": "pop",
        "duration": 180
    }

    print("正在调用 Minimax API 生成音乐...")
    response = requests.post(url, headers=headers, json=payload, timeout=60)

    if response.status_code == 200:
        result = response.json()
        print(f"API 响应: {json.dumps(result, indent=2, ensure_ascii=False)}")
        return result
    else:
        print(f"错误: {response.status_code}")
        print(response.text)
        return None

if __name__ == "__main__":
    generate_music()
