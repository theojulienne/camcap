#!/usr/bin/env python

from distutils.core import setup, Extension

import glob

_camcap = Extension(
	'_camcap',
	sources = glob.glob( 'src/*.c' ) + glob.glob( 'src/qtkit/*.m' ),
	include_dirs = ['src/', 'src/qtkit/'],
	extra_compile_args=['-DQTKIT_INPUT'],
	extra_link_args=['-framework', 'QTKit', '-framework', 'Foundation', '-framework', 'QuartzCore'],
)

setup(
	name='CamCap',
	version='0.1',
	description='CamCap',
	author='Theo Julienne',
	author_email='theo.julienne+camcap@gmail.com',
	py_modules=['camcap'],
	package_dir={'': 'python'},
	ext_modules=[_camcap]
)