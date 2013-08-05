import Image
import sys
import optparse
import glob
import os

"python atlas.py -d dirname"
 
def Atlas(filenames, outname, size, dir):
 
    #open all of the images
    ims = [(f, Image.open(dir+'/'+f)) for f in filenames]
 
    #sort by image area
    ims = sorted(ims, key = lambda im : im[1].size[0]*im[1].size[1], reverse=True)
 
    #Free boxes are empty space in our output image set
    #the freebox tuple format is: outidx, x, y, w, h
    freeboxes = [(0, 0, 0, size, size)]
    numoutimages = 1
 
    #Full boxes are areas where we have placed images in the atlas
    #the full box tuple format is: image, outidx, x, y, w, h, filename
    fullboxes = []
 
    #Do the actual atlasing by sticking the largest images we can have into the smallest valid free boxes
    for imageinfo in ims:
        im = imageinfo[1]
        if im.size[0] > size or im.size[1] > size:
            print "Error: image " + imageinfo[0] + " is larger than the atlas size!"
            sys.exit(2)
 
        inserted = False;
 
        while not inserted:
            for idx, fb in enumerate(freeboxes):
                #find the smallest free box that will contain this image
                if fb[3] >= im.size[0] and fb[4] >= im.size[1]:
                    #we found a valid spot! Remove the current freebox, and split the leftover space into (up to) two new freeboxes
                    del freeboxes[idx]
                    if fb[3] > im.size[0]:
                        freeboxes.append( (fb[0], fb[1]+im.size[0], fb[2], fb[3]-im.size[0], im.size[1]) )
 
                    if fb[4] > im.size[1]:
                        freeboxes.append( (fb[0], fb[1], fb[2]+im.size[1], fb[3], fb[4] - im.size[1]) )
 
                    # keep this sorted!
                    freeboxes = sorted(freeboxes, key = lambda fb : fb[3]*fb[4])
                    fullboxes.append( (im, fb[0], fb[1], fb[2], im.size[0], im.size[1], imageinfo[0]))
                    inserted = True
                    break;
            if not inserted:
                #oh crap - there isn't room in any of our free boxes, so we have to add a new output image
                freeboxes.append((numoutimages, 0, 0, size, size))
                numoutimages += 1
 
    #now that we've figured out where everything goes, make the output images and blit the source images to the approriate locations
    outimages = [Image.new("RGBA", (int(size),int(size))) for i in range(0,int(numoutimages))]
    for fb in fullboxes:
        outimages[fb[1]].paste(fb[0], (fb[2], fb[3]))        
 
    #save the output images
    for idx, outimage in enumerate(outimages):
        outimage.save(outname + "-" + str(idx) + ".png")
        # w, h = outimage.size
        # outimage.resize((w/2, h/2), Image.ANTIALIAS).save(outname + "-" + str(idx) + ".png")
 
    #write out an xml file that says where everything ended up
    xmlfile = open(outname + ".atlas", "w")
    xmlfile.write("<images>\n")
 
    for fb in fullboxes:
        xmlfile.write('\t<image name="{0}" file="{1}_{2}_atlas.png" x="{3}" y ="{4}" w="{5}" h="{6}" />\n'.format(fb[6],outname, fb[1], fb[2], fb[3], fb[4], fb[5] ))
        
    xmlfile.write("</images>\n")
    print ("It all fit into " + str(numoutimages) + " images!")
 
def main(argv):
    parser = optparse.OptionParser(description='Combine multiple image files into a texture atlas')
    parser.add_option('-s', '--size', help="atlas size (length and width)", dest="size", default=512, type=float)
    parser.add_option('-o', '--output', help="output name", dest="output", default="atlas", type='string')
    (options, arguments) = parser.parse_args()
 
    if (len(arguments) == 0):
        print "Input file(s) not specified"
        sys.exit(2)
 
    infiles = []
    for f in arguments:
        infiles.extend(glob.glob(f))
    infiles = list(set(infiles))        
 
    print "Atlasing " + str(len(infiles)) + " files:"
    Atlas(infiles, options.output, options.size)

def main1(argv):
    parser = optparse.OptionParser(description='Combine multiple image files into a texture atlas')
    parser.add_option('-s', '--size', help="atlas size (length and width)", dest="size", default=512, type=float)
    parser.add_option('-d', '--dir', help="input dir", dest="indir", type='string')
    (options, arguments) = parser.parse_args() 

    indir = options.indir
    if indir == None:
        print 'need -d'
        sys.exit(2)
    infiles = []
    for a, b, c in os.walk(indir):
        if (a==indir):
            for f in c:
                fname, ext = os.path.splitext(f)
                if ( ext == '.png' ):
                    infiles.append(f)
                
            print "Atlasing " + str(len(infiles)) + " files:"
            print infiles
            outname = indir
            Atlas(infiles, outname, options.size, indir)

if __name__ == "__main__":
    main1(sys.argv[1:])