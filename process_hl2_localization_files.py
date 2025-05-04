import os
import shutil
import re

def process_file(source_path, target_dir):
    # 读取源文件内容
    with open(source_path, 'r', encoding='utf-16 le') as f:
        content = f.read()
    
    # 找到第二个 { 的位置
    second_brace_pos = [m.start() for m in re.finditer(r'\{', content)][1]
    
    # 提取所有 HL2_ 和 hl2_ 开头的行
    hl2_lines = []
    for line in content.split('\n'):
        if line.strip().startswith('"HL2_') or line.strip().startswith('"hl2_'):
            # 替换前缀
            new_line = line.replace('"HL2_', '"DGLabIEMod_').replace('"hl2_', '"DGLabIEMod_')
            hl2_lines.append(new_line)
    
    # 在第二个 { 后插入新行
    if hl2_lines:
        new_content = content[:second_brace_pos + 1] + '\n' + '\n'.join(hl2_lines) + content[second_brace_pos + 1:]
    else:
        new_content = content
    
    # 创建目标文件名
    target_filename = os.path.basename(source_path).replace('hl2_', 'dglabiemod_')
    target_path = os.path.join(target_dir, target_filename)
    
    # 写入新文件
    with open(target_path, 'w', encoding='utf-16 le') as f:
        f.write(new_content)

def main():
    # 源目录和目标目录
    source_dir = r"C:\Program Files (x86)\Steam\steamapps\common\Half-Life 2\hl2\resource"
    target_dir = "game/mod_hl2/resource"
    
    # 确保目标目录存在
    os.makedirs(target_dir, exist_ok=True)
    
    # 处理所有匹配的文件
    for filename in os.listdir(source_dir):
        if filename.startswith('hl2_') and filename.endswith('.txt'):
            source_path = os.path.join(source_dir, filename)
            process_file(source_path, target_dir)
            print(f"已处理文件: {filename}")

if __name__ == "__main__":
    main() 