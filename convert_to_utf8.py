import os
import chardet
from multiprocessing import Pool, cpu_count

# 需要转换的文件扩展名
CODE_EXTS = {'.py', '.cpp', '.h', '.c', '.js', '.ts', '.java', '.cs', '.html', '.css'}

def detect_encoding(file_path):
    with open(file_path, 'rb') as f:
        raw = f.read(4096)
        result = chardet.detect(raw)
        return result['encoding'] or 'utf-8'

def convert_file(file_path):
    try:
        encoding = detect_encoding(file_path)
        with open(file_path, 'r', encoding=encoding, errors='ignore') as f:
            lines = f.readlines()
        # 去除每行末尾多余的换行符，再统一加上\n，避免多余空行
        lines = [line.rstrip('\r\n') for line in lines]
        with open(file_path, 'w', encoding='utf-8', newline='\n') as f:
            f.write('\n'.join(lines))
        print(f"已转换: {file_path} ({encoding} -> utf-8)")
    except Exception as e:
        print(f"转换失败: {file_path}, 错误: {e}")

def get_all_code_files(root):
    code_files = []
    for dirpath, _, filenames in os.walk(root):
        for filename in filenames:
            ext = os.path.splitext(filename)[1].lower()
            if ext in CODE_EXTS:
                code_files.append(os.path.join(dirpath, filename))
    return code_files

if __name__ == '__main__':
    src_dir = 'src'
    files = get_all_code_files(src_dir)
    print(f"共找到{len(files)}个代码文件，开始转换为utf-8编码...")
    with Pool(cpu_count()) as pool:
        pool.map(convert_file, files)
    print("全部转换完成。")
