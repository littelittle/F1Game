# -*- coding: utf-8 -*-

def calculate_mean_from_obj(file_path):
    """
    从一个.obj文件中读取所有的顶点，并计算这些点的坐标均值。

    参数:
    file_path (str): .obj文件的路径。

    返回:
    tuple: 一个包含 (平均x, 平均y, 平均z) 的元组，如果文件不存在或没有顶点则返回None。
    """
    vertices = []
    total_x, total_y, total_z = 0.0, 0.0, 0.0

    try:
        with open(file_path, 'r') as f:
            for line in f:
                # .obj 文件中，顶点信息以 'v ' 开头
                if line.strip().startswith('v '):
                    try:
                        # 分割行并提取坐标，通常是 "v x y z" 的格式
                        parts = line.split()
                        x = float(parts[1])
                        y = float(parts[2])
                        z = float(parts[3])
                        
                        vertices.append((x, y, z))
                        total_x += x
                        total_y += y
                        total_z += z
                    except (IndexError, ValueError) as e:
                        print(f"警告：无法解析行 '{line.strip()}'，已跳过。错误: {e}")

    except FileNotFoundError:
        print(f"错误：文件 '{file_path}' 不存在。")
        return None
    except Exception as e:
        print(f"读取文件时发生未知错误: {e}")
        return None

    num_vertices = len(vertices)

    if num_vertices == 0:
        print("文件中没有找到任何顶点。")
        return None

    mean_x = total_x / num_vertices
    mean_y = total_y / num_vertices
    mean_z = total_z / num_vertices

    print(f"成功读取文件: {file_path}")
    print(f"共找到 {num_vertices} 个顶点。")
    print(f"坐标均值为: (x: {mean_x:.6f}, y: {mean_y:.6f}, z: {mean_z:.6f})")

    return (mean_x, mean_y, mean_z)

if __name__ == '__main__':
    # 提示用户输入文件路径
    # 在Windows上，路径可以是 "C:\\path\\to\\your\\file.obj"
    # 在macOS或Linux上，路径可以是 "/path/to/your/file.obj"
    obj_file_path = 'C:/Users/27217/playground/Cpp/F1/assets/F1_car/newC44/rearleft/rearleft.obj'
    
    # 调用函数并执行计算
    calculate_mean_from_obj(obj_file_path)