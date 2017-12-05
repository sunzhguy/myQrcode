/*------------------------------------------------------------------------
 *  Copyright 2007-2009 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/
#if 0

#include <config.h>
#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#endif
#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>

#include <zbar.h>
//#include "test_images.h"

#ifdef _WIN32
struct timespec {
    time_t tv_sec;
    long tv_nsec;
};
#endif

zbar_video_t *video;
zbar_image_scanner_t *scanner = NULL;
static inline int get_timer_now ()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    return(now.tv_sec * 1000 + now.tv_usec / 1000);
}


#if 0
int _sun_process_image (zbar_processor_t *proc,
                         zbar_image_t *img)
{


        uint32_t format = zbar_image_get_format(img);
        printf("processing: %.4s(%08" PRIx32 ") %dx%d @%p\n",
                (char*)&format, format,
                zbar_image_get_width(img), zbar_image_get_height(img),
                zbar_image_get_data(img));
        zbar_image_t *tmp = zbar_image_convert(img, fourcc('Y','8','0','0'));
        if(!tmp)
            goto error;

        if(proc->syms) {
            zbar_symbol_set_ref(proc->syms, -1);
            proc->syms = NULL;
        }
        zbar_image_scanner_recycle_image(proc->scanner, img);
        int nsyms = zbar_scan_image(proc->scanner, tmp);/*zbar_processor_s; zbar_image_scanner_s;zbar_scanner_s,zbar_scanner_s*/
        _zbar_image_swap_symbols(img, tmp);
        zbar_image_destroy(tmp);
        tmp = NULL;
        if(nsyms < 0)
            goto error;
        proc->syms = zbar_image_scanner_get_results(proc->scanner);
        if(proc->syms)
            zbar_symbol_set_ref(proc->syms, 1);
#if 0
        if(_zbar_verbosity >= 8) {
            const zbar_symbol_t *sym = zbar_image_first_symbol(img);
			int sunCnt=0;
			 sunprintf(8, "=========================%p=====================\r\n",sym);
            while(sym) {
                zbar_symbol_type_t type = zbar_symbol_get_type(sym);
                int count = zbar_symbol_get_count(sym);
				#if SUNZGHUY
                zprintf(8, "%s: %s (%d pts) (dir=%d) (q=%d) (%s)\n",
                        zbar_get_symbol_name(type),
                        zbar_symbol_get_data(sym),
                        zbar_symbol_get_loc_size(sym),
                        zbar_symbol_get_orientation(sym),
                        zbar_symbol_get_quality(sym),
                        (count < 0) ? "uncertain" :
                        (count > 0) ? "duplicate" : "new");
			  #else
			  zprintf(8, "%s:  (%d pts) (dir=%d) (q=%d) (%s)\n",
									 zbar_get_symbol_name(type),
									 zbar_symbol_get_loc_size(sym),
									 zbar_symbol_get_orientation(sym),
									 zbar_symbol_get_quality(sym),
									 (count < 0) ? "uncertain" :
									 (count > 0) ? "duplicate" : "new");

			  #endif

						
                sym = zbar_symbol_next(sym);
				sunCnt++;
            }
			zprintf(21, "=========================%p===sunCnt[%d]============nsyms[%d]======\r\n",sym,sunCnt,nsyms);
        }
#endif
        if(nsyms) {
            /* FIXME only call after filtering */
            _zbar_mutex_lock(&proc->mutex);
            _zbar_processor_notify(proc, EVENT_OUTPUT);
            _zbar_mutex_unlock(&proc->mutex);
            if(proc->handler)
            	{
            	 zprintf(20, "sunzhguy  enter...:nsyms[%d]\n",nsyms);
                 proc->handler(img, proc->userdata);
            	}
           }
    
    return 0;

error:
	printf("Eroor:unknown image format,%s\n",__func__);
    return -1;
}
#endif

int main (int argc, char *argv[])
{
    zbar_set_verbosity(1);

    const char *dev = "";
    uint32_t vidfmt = 0;
    if(argc > 1) {
        dev = argv[1];

        if(argc > 2) {
            int n = strlen(argv[2]);
            if(n > 4)
                n = 4;
            memcpy((char*)&vidfmt, argv[2], n);
        }
    }
    if(!vidfmt)
        vidfmt = fourcc('Y','U','Y','V');

    video = zbar_video_create();
    if(!video) {
        fprintf(stderr, "unable to allocate memory?!\n");
        return(1);
    }

    zbar_video_request_size(video, 640, 480);

    if(zbar_video_open(video, dev)) {
        zbar_video_error_spew(video, 0);
        fprintf(stderr,
                "ERROR: unable to access your video device\n"
                "this program requires video capture support using"
                " v4l version 1 or 2 or VfW\n"
                "    - is your video device located at \"%s\"?\n"
                "    - is your video driver installed? (check dmesg)\n"
                "    - make sure you have the latest drivers\n"
                "    - do you have read/write permission to access it?\n"
                "    - is another application is using it?\n"
                "    - does the device support video capture?\n"
                "    - does the device work with other programs?\n",
                dev);
        return(1);
    }
    fprintf(stderr, "opened video device: %s (fd=%d)\n",
            dev, zbar_video_get_fd(video));
    fflush(stderr);

    if(zbar_video_init(video, vidfmt)) {
        fprintf(stderr, "ERROR: failed to set format: %.4s(%08x)\n",
                (char*)&vidfmt, vidfmt);
        return(zbar_video_error_spew(video, 0));
    }

    if(zbar_video_enable(video, 1)) {
        fprintf(stderr, "ERROR: starting video stream\n");
        return(zbar_video_error_spew(video, 0));
    }
    fprintf(stderr, "started video stream...\n");
    fflush(stderr);

    zbar_image_t *image = zbar_video_next_image(video);
    if(!image) {
        fprintf(stderr, "ERROR: unable to capture image\n");
        return(zbar_video_error_spew(video, 0));
    }
    uint32_t format = zbar_image_get_format(image);
    unsigned width = zbar_image_get_width(image);
    unsigned height = zbar_image_get_height(image);
    const uint8_t *data = zbar_image_get_data(image);
    fprintf(stderr, "captured image: %d x %d %.4s @%p\n",
            width, height, (char*)&format, data);
    fflush(stderr);

    zbar_image_destroy(image);

    fprintf(stderr, "\nstreaming 100 frames...\n");
    fflush(stderr);

    struct timespec start, end;
#if _POSIX_TIMERS > 0
    clock_gettime(CLOCK_REALTIME, &start);
#else
    struct timeval ustime;
    gettimeofday(&ustime, NULL);
    start.tv_nsec = ustime.tv_usec * 1000;
    start.tv_sec = ustime.tv_sec;
#endif
	scanner = zbar_image_scanner_create();
	
	   /* configure the reader */
	zbar_image_scanner_set_config(scanner, 0, ZBAR_CFG_ENABLE, 1);

    int i;
    for(i = 0; i < 100000; i++) {
		long startTim=get_timer_now();
        zbar_image_t *image = zbar_video_next_image(video);
        if(!image) {
            fprintf(stderr, "ERROR: unable to capture image\n");
            return(zbar_video_error_spew(video, 0));
        }
		zbar_image_t *tmp = zbar_image_convert(image, fourcc('Y','8','0','0'));
		 //zbar_image_scanner_recycle_image(scanner, image);

		 zbar_scan_image(scanner, tmp);
		 
		 zbar_image_set_symbols(image,zbar_image_get_symbols(tmp));
		 //const zbar_symbol_t *symbol = zbar_image_first_symbol(image);
		 zbar_symbol_set_t *syms=zbar_image_scanner_get_results(scanner);
            
			const zbar_symbol_t *symbol = zbar_image_first_symbol(image);
			for(; symbol; symbol = zbar_symbol_next(symbol)) {
				/* do something useful with results */
				zbar_symbol_type_t typ = zbar_symbol_get_type(symbol);
				const char *data = zbar_symbol_get_data(symbol);
				printf("decoded %s symbol \"%s\"\n",
					   zbar_get_symbol_name(typ), data);
			}            
       //zbar_image_destroy(tmp);
	   zbar_image_destroy(image);
	   long endTim=get_timer_now();
	   printf("Spend Time ========%d\n",endTim-startTim);

	}

#if _POSIX_TIMERS > 0
    clock_gettime(CLOCK_REALTIME, &end);
#else
    gettimeofday(&ustime, NULL);
    end.tv_nsec = ustime.tv_usec * 1000;
    end.tv_sec = ustime.tv_sec;
#endif
    double ms = (end.tv_sec - start.tv_sec +
                 (end.tv_nsec - start.tv_nsec) / 1000000000.);
    double fps = i / ms;
    fprintf(stderr, "\nprocessed %d images in %gs @%gfps\n", i, ms, fps);
    fflush(stderr);

    if(zbar_video_enable(video, 0)) {
        fprintf(stderr, "ERROR: while stopping video stream\n");
        return(zbar_video_error_spew(video, 0));
    }

    fprintf(stderr, "\ncleaning up...\n");
    fflush(stderr);
    zbar_video_destroy(video);
    zbar_image_scanner_destroy(scanner);
   // if(test_image_check_cleanup())
   //     return(32);

    fprintf(stderr, "\nvideo support verified\n\n");
    return(0);
}
#endif

#include <stdio.h>
#include <zbar.h>
//#include "gpio/gpio.h"
#include <sys/time.h> 
#include <stdlib.h>


int beep_Statue=-1;
#define VIDEO_USERPTR 3

static inline int get_timer_now ()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    return(now.tv_sec * 1000 + now.tv_usec / 1000);
}

static void my_handler (zbar_image_t *image,
                        const void *userdata)
{
int n=0;
    /* extract results */
    const zbar_symbol_t *symbol = zbar_image_first_symbol(image);
    for(; symbol; symbol = zbar_symbol_next(symbol)) {
        /* do something useful with results */
        zbar_symbol_type_t typ = zbar_symbol_get_type(symbol);
        const char *data = zbar_symbol_get_data(symbol);
        printf("decoded %s symbol \"%s\"\n",
               zbar_get_symbol_name(typ), data);
		n++;
    }

#if 0
	if(!beep_Statue&&n)
    		{
    		  Beep();
    		}
#endif
}



int main (int argc, char **argv)
{
	int verbose=0;

    const char *device = "/dev/video0";

	 if(argc > 1)
        device = argv[1];
	 if(argc >2)
	 	{
	 	  verbose=atoi(argv[2]);
	 	}
    zbar_set_verbosity(verbose);
    /* create a Processor */
    zbar_processor_t *proc = zbar_processor_create(1);//zbar_processor_s
    // beep_Statue= Beep_Init();
	 //printf("beep_Statue==%d\n",beep_Statue);
    /* configure the Processor */
    //zbar_processor_set_config(proc, 0, ZBAR_CFG_ENABLE, 1);

    /* initialize the Processor */
    zbar_processor_request_size(proc, 1280, 720);
	zbar_processor_force_format(proc, *(int*)("YUYV"), *(int*)("YUYV"));
	zbar_processor_request_iomode(proc,VIDEO_USERPTR);
    zbar_processor_init(proc, device, 1);

    /* setup a callback */
    zbar_processor_set_data_handler(proc, my_handler, NULL);

    /* enable the preview window */
    //zbar_processor_set_visible(proc, 1);
    zbar_processor_set_active(proc, 1);

    /* keep scanning until user provides key/mouse input */
    //zbar_processor_user_wait(proc, -1);

	while(1)
		{
		   long t1=get_timer_now();
		   zbar_Handle(proc);
		   long t2=get_timer_now();
		   if(verbose==2||verbose==19)
		   printf("the...time==%d\n",t2-t1);
		}
    /* clean up */
    zbar_processor_destroy(proc);

    return(0);
}

