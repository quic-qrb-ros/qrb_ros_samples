from setuptools import find_packages, setup

package_name = 'sample_object_detection'

setup(
    name=package_name,
    version='0.1.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        ('share/' + package_name + '/launch', ['launch/launch.py']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='Fulan Lin',
    maintainer_email='quic_fulaliu@quicinc.com',
    description='object detection with Yolo model',
    license='BSD-3-Clause-Clear',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'image_converter_node = sample_object_detection.image_converter_node:main',
        ],
    },
)
