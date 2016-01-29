#!/usr/bin/python
import os.path, sys
import matplotlib as mpl
mpl.rcParams['toolbar'] = 'None'
mpl.use("QT4Agg")
import sys, time, collections
from matplotlib import pyplot as plt
import numpy as np
from matplotlib.animation import FuncAnimation
import matplotlib.cbook as cbook
import matplotlib.ticker as mtick

# Demo tunables
# if you have trouble getting it fullscreen, define the screen resolution here
#SCREEN=(1920,1200)

# frequency (MHz)
try:	FREQ=int(open("/mppa/board0/mppa0/chip_freq").read()) # try to get it from the driver
except:	FREQ=400

TITLE_FONT={'family':'sans-serif','color':'#0f588e','weight':'bold','size':30}

BW_HISTORY=10
BW_MIN=39
BW_MAX=40

PPS_HISTORY=100
PPS_MIN=13
PPS_MAX=14

class MCAnim(object):
	def __init__(self, fig, ax1, ax2, ax3):
		ax2.set_xlim((0, BW_HISTORY))
		ax3.set_xlim((0, PPS_HISTORY))

		self.fig = fig
		self.ax1 = ax1
		self.ax2 = ax2
		self.ax3 = ax3

	def init(self):
		#self.bw_history = self.ax2.bar(np.arange(BW_HISTORY), np.zeros((BW_HISTORY,1)), 1, color='#30a2da')
		self.bw_history = np.zeros(BW_HISTORY)
		self.pps_history = np.zeros(PPS_HISTORY)
		#return []

	def __call__(self, i):
		# Cluster15@0.0: 00: STATS: 1085266 pkts in 600012289 cycles
                line = sys.stdin.readline().split()
		try:
			assert(line[2] == 'STATS:')
		except:
			print 'STDOUT:', line
			return
                pkt, cycles = int(line[3]), int(line[6])
                pps = float(pkt) * (FREQ * 1e6) / cycles
                bw = (pps * (256 +42) * 8)
		pps, bw = 16 * pps, 16 * bw
		pps, bw = pps/1e6, bw/1e9

		print line, pps, bw

		if pps == 0 and self.pps_history[-1] > 0: sys.exit(1)

#		scale = None
#		for i in range(BW_HISTORY-1):
#			h = self.bw_history[i+1].get_height()
#			if 0 == h: continue
#			ymin, ymax = self.ax2.get_ylim()
#			if h < ymin: scale = (0.9999*h, ymax)
#			elif h > ymax: scale = (ymin, 1.0005*h)
#			self.bw_history[i].set_height(h)
#		self.bw_history[BW_HISTORY-1].set_height(bw)

                self.bw_history = np.roll(self.bw_history, -1)
                self.bw_history[-1] = bw
                self.ax2.clear()
                bw_line = self.ax2.plot(self.bw_history, linewidth=4, color='g')
		self.ax2.set_ylim((BW_MIN, BW_MAX))
		self.ax2.set_title('Bandwidth (Gbps)')
		self.ax2.set_xticklabels([])
		self.ax2.set_xlabel('')
		self.ax2.set_ylabel('Gbps')

		self.pps_history = np.roll(self.pps_history, -1)
                self.pps_history[-1] = pps
                self.ax3.clear()
                pps_line = self.ax3.plot(self.pps_history, linewidth=4)
		self.ax3.set_ylim((PPS_MIN, PPS_MAX))
		self.ax3.set_title('Millions of Packets Per Second')
		self.ax3.set_xticklabels([])
		self.ax3.set_xlabel('')
		self.ax3.set_ylabel('MPPS')


#		if scale:
#			self.ax2.set_ylim(scale)
#			plt.draw()
#			return []

                #pps_line.extend(bw_line)
                #pps_line.extend(self.ax2.get_xaxis())
		#return pps_line


fig = plt.figure(facecolor='white')
plt.style.use('bmh')

ax4 = plt.subplot2grid((2,2), (1,1))
ax3 = plt.subplot2grid((2,2), (1,0))
ax2 = plt.subplot2grid((2,2), (0,1))
ax1 = plt.subplot2grid((2,2), (0,0))

plt.suptitle('IPsec on Konic80 (%iMHz)' % FREQ, fontdict=TITLE_FONT)

ax1.set_frame_on(False)
ax1.axes.get_xaxis().set_visible(False)
ax1.axes.get_yaxis().set_visible(False)
path = os.path.dirname(sys.argv[0])
path = os.path.abspath(path)
image_file = cbook.get_sample_data(path + '/konic80.png')
image = plt.imread(image_file)
ax1.imshow(image)
ax4.set_frame_on(False)
ax4.axes.get_xaxis().set_visible(False)
ax4.axes.get_yaxis().set_visible(False)
image_file = cbook.get_sample_data(path + '/ipsec.png')
image = plt.imread(image_file)
ax4.imshow(image)

mng = plt.get_current_fig_manager()
mng.full_screen_toggle()
#fig.set_size_inches(float(SCREEN[0])/fig.dpi, float(SCREEN[1])/fig.dpi, forward=True)

mca = MCAnim(fig, ax1, ax2, ax3)
anim = FuncAnimation(fig, mca, frames=100000, init_func=mca.init, interval=10, blit=False)
plt.show()
