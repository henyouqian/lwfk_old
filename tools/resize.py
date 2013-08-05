import Image
import sys
import os

outConf = (	('iphone/iTunesArtwork', 512),
			('iphone/Icon-72.png', 72),
			('iphone/Icon-Small-50.png', 50),
			('iphone/Icon-Small.png', 29),
			('iphone/Icon-Small@2x.png', 58),
			('iphone/Icon.png', 57),
			('iphone/Icon@2x.png', 114),
			('Android/drawable-hdpi/icon.png', 72),
			('Android/drawable-mdpi/icon.png', 48),
			('Android/drawable-ldpi/icon.png', 36),
			)
			
def buildDir(rootDir):
	if (os.path.exists(rootDir)==False):
		os.mkdir(rootDir)
	if (os.path.exists(rootDir+'/iPhone')==False):
		os.mkdir(rootDir+'/iPhone')
	if (os.path.exists(rootDir+'/Android')==False):
		os.mkdir(rootDir+'/Android')
	if (os.path.exists(rootDir+'/Android/drawable-hdpi')==False):
		os.mkdir(rootDir+'/Android/drawable-hdpi')
	if (os.path.exists(rootDir+'/Android/drawable-mdpi')==False):
		os.mkdir(rootDir+'/Android/drawable-mdpi')
	if (os.path.exists(rootDir+'/Android/drawable-ldpi')==False):
		os.mkdir(rootDir+'/Android/drawable-ldpi')

if __name__ == '__main__':
	argv = sys.argv
	if (len(argv)==1):
		print '''example: 
	do files: python resize.py a.png b.png c.png
	do files in current directory: python resize.py -d
	do files in any directory: python resize.py -d inputdir
	make files half size in current directory: python resize.py -h inputdir
				'''
	if ('-d' in argv):
		dir = os.getcwd()
		if ( argv[-1] != '-d' ):
			dir = argv[argv.index('-d')+1]
			
		for a, b, c in os.walk(dir):
			if (a==dir):
				for infile in c:
					fileName, e = os.path.splitext(infile)
					if ( e!='.png' ):
						continue
					print "Process ", fileName
					outdir = dir+'/'+fileName
					buildDir(outdir)
					for conf in outConf:
						f, sz = conf
						try:
							Image.open(infile).resize((sz, sz), Image.ANTIALIAS).save(outdir+'/'+f, 'png')
						except IOError:
							print "cannot convert", infile, f
				print "Done!"
				break
	elif ('-h' in argv):
		dir = os.getcwd()
		if ( argv[-1] != '-h' ):
			dir = argv[argv.index('-h')+1]
		outdir = dir+'/halfout'
		if (os.path.exists(outdir)==False):
			os.mkdir(outdir)
		for a, b, c in os.walk(dir):
			if (a==dir):
				for infile in c:
					fileName, e = os.path.splitext(infile)
					if ( e!='.png' ):
						continue
					print "Process ", fileName
					try:
						im = Image.open(dir+'/'+infile)
						w, h = im.size
						im.resize((w/2, h/2), Image.ANTIALIAS).save(outdir+'/'+infile)
					except IOError:
						print "cannot convert", infile
				print "Done!"
				break
	else:
		for infile in sys.argv[1:]:
			fileName, e = os.path.splitext(infile)
			buildDir(fileName)
			print "Process ", fileName
			for conf in outConf:
				f, sz = conf
				try:
					Image.open(infile).resize((sz, sz), Image.ANTIALIAS).save(fileName+'/'+f, 'png')
				except IOError:
					print "cannot convert", infile, f
		print 'Done'
