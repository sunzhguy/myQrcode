#include <stdio.h>
#include <stdlib.h>
#include <zbar.h>
#include <string.h>
#include <sys/time.h> 
#include <unistd.h>
zbar_image_scanner_t *scanner = NULL;

/* to complete a runnable example, this abbreviated implementation of
 * get_data() will use libpng to read an image file. refer to libpng
 * documentation for details
 */

/////image  yuyv size  image width height
static void get_data (const char *name,
                      int *width, int *height,
                      void **raw)
{
    FILE *file = fopen(name, "rb");
	int fileSize=0;
    if(!file) exit(2);
	//int size=fread(file,*raw,width*height);
	

    fseek(file,0,SEEK_END); //定位到文件末   
    if ((fileSize = ftell(file))<1)//文件长度  
    {  
        fclose(file);  
        exit(1); 
    }  
	fseek(file,0,SEEK_SET); //定位到文件末   
	printf("the file size==%d\n",fileSize);
      *raw =malloc(fileSize);
	  memset(*raw,0,fileSize);
    //char* data = (char *)malloc(sizeof(char)*(nFileLen+1)); 
    if (NULL == *raw)  
    {  
        fclose(file);  
         exit(0);  
    }
	int rsize=fread(*raw, fileSize, 1, file);
	
	printf("the file rd size==%d,%p\n",rsize, *raw);


}
static inline int get_timer_now ()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    return(now.tv_sec * 1000 + now.tv_usec / 1000);
}

int main (int argc, char **argv)
{
    if(argc < 2) return(1);

    /* create a reader */
    scanner = zbar_image_scanner_create();

    /* configure the reader */
    //zbar_image_scanner_set_config(scanner, 0, ZBAR_CFG_ENABLE, 1);

    /* obtain image data */
    int width = 640, height = 480;
    void *raw = NULL;
    get_data(argv[1], &width, &height, &raw);

    while(1)
{
    /* wrap image data */
    zbar_image_t *image = zbar_image_create();
	
	zbar_image_set_format(image, *(int*)"YUYV");
    zbar_image_set_size(image, width, height);
	printf("the image=%p\n",image);
    zbar_image_set_data(image, raw, width * height, zbar_image_free_data);
	printf("the image=%p\n",image);

	long tm_start=get_timer_now ();
	zbar_image_t *tmp=zbar_image_convert(image, *(int*)"Y800");
	long tm_end=get_timer_now ();
	printf("the image=%p---------------------ttt===%lu\n",tmp,tm_end-tm_start);
    image=tmp;
    /* scan the image for barcodes */

	tm_start=get_timer_now ();
    int n = zbar_scan_image(scanner, image);
	tm_end=get_timer_now ();
	printf("the scanN=%d---------------------ttt===%lu\n",n,tm_end-tm_start);
    /* extract results */
    const zbar_symbol_t *symbol = zbar_image_first_symbol(image);
	printf("the....%p\n",symbol);
    for(; symbol; symbol = zbar_symbol_next(symbol)) {
        /* do something useful with results */
	
        zbar_symbol_type_t typ = zbar_symbol_get_type(symbol);
        const char *data = zbar_symbol_get_data(symbol);
        printf("decoded %s symbol \"%s\"\n",
               zbar_get_symbol_name(typ), data);
    }

    /* clean up */
    zbar_image_destroy(image);
    usleep(50);
}
    zbar_image_scanner_destroy(scanner);
    return(0);
}
