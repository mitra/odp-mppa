#!/bin/sh
die()
{
	echo "$*"
	exit 1
} >&2

fc17_wa()
{
	sudo yum install agg-devel pygtk2-devel \
		python-basemap python-sphinx tk-devel \
		wxPython-devel bakefile geos pycairo-devel \
		pygobject2-codegen pygobject2-devel \
		pygobject2-doc pygtk2-codegen pygtk2-doc \
		python-babel python-basemap-data \
		python-docutils python-empy python-jinja2 \
		python-pygments tcl-devel wxGTK-devel \
		wxGTK-gl wxGTK-media wxPython
}

sudo yum-builddep python-matplotlib || { fc17_wa || die ; }
wget 'https://bootstrap.pypa.io/get-pip.py' || die
python ./get-pip.py --user || die
rm -f 'get-pip.py'
easy_install --user --upgrade distribute
pip install --user --upgrade matplotlib
