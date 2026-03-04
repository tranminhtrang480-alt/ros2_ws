from launch import LaunchDescription
from launch_ros.actions import Node
# 封装终端指令相关类--------------
# from launch.actions import ExecuteProcess
# from launch.substitutions import FindExecutable
# 参数声明与获取-----------------
# from launch.actions import DeclareLaunchArgument
# from launch.substitutions import LaunchConfiguration
# 文件包含相关-------------------
# from launch.actions import IncludeLaunchDescription
# from launch.launch_description_sources import PythonLaunchDescriptionSource
# 分组相关----------------------
# from launch_ros.actions import PushRosNamespace
# from launch.actions import GroupAction
# 事件相关----------------------
# from launch.event_handlers import OnProcessStart, OnProcessExit
# from launch.actions import ExecuteProcess, RegisterEventHandler,LogInfo
# 获取功能包下share目录路径-------
# from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
        # 获取detect包的路径
    detect_pkg_dir = get_package_share_directory('detect')

    # 相机节点参数文件路径
    usb_cam_params = os.path.join(detect_pkg_dir, 'config', 'params.yaml')

    # 相机节点
    usb_cam_node = Node(
        package='usb_cam',
        executable='usb_cam_node_exe',
        name='usb_cam',
        output='screen',
        parameters=[usb_cam_params]  # 可选：使用参数文件
    )

    # 检测节点
    detect_node = Node(
        package='detect',
        executable='detect_node',
        name='detect_node',
        output='screen',
        parameters=[{
            'templates_path': os.path.join(detect_pkg_dir, 'templates')
        }]
    )
    return LaunchDescription([usb_cam_node,detect_node])