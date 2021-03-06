int pixel_buffer_start; // global variable



// code not shown for clear_screen() and draw_line() subroutines

void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)((pixel_buffer_start) + (y << 10) + (x << 1)) = line_color;
}

//draws a line from x1, y1 to x2, y2. Requires plotpixel to run
//assumes math.h not available
void draw_line(int x1, int y1, int x2, int y2, int colour) {
	int is_steep = 0;
	int deltay = y2 - y1;
	int deltax = x2 - x1;
	int error = (-1)*(deltax / 2);

	//checking if deltay is positive
	if (deltay < 0) {
		deltay = deltay * (-1);
	}

	//finds it the slope is greater than 1
	if (((deltax >= 0) && (deltay > deltax)) || ((deltax < 0) && (deltay > (deltax * (-1))))) {
		is_steep = 1;
	}
	
	//if slope greater than one trades variables to switch from y/x to x/y
	if (is_steep) {
		int temp = x1;
		x1 = y1;
		y1 = temp;

		temp = x2;
		x2 = y2;
		y2 = temp;
	}
	
	//ensures x2 is greater than x1
	if (x1 > x2) {
		int temp = x1;
		x1 = x2;
		x2 = temp;

		temp = y1;
		y1 = y2;
		y2 = temp;
	}
	deltay = y2 - y1;
	deltax = x2 - x1;
	error = (-1)*(deltax / 2);
	int y = y1;
	int y_step;

	if (deltay < 0) {
		deltay = deltay * (-1);
	}

	//determines which way to increment y
	if (y1 < y2) {
		y_step = 1;
	}
	else {
		y_step = -1;
	}

	int x = 0;

	//plots all the pixels in the line
	for (x = x1; x <= x2; x++) {
		if (is_steep) {
			plot_pixel(y, x, colour);
		}
		else {
			plot_pixel(x, y, colour);
		}

		error = error + deltay;

		if (error >= 0) {
			y = y + y_step;
			error = error - deltax;
		}
	}	
}

//writes all pixels to black
void clear_screen() {
	int x = 0;
	
	for (x = 0; x < 320; x++) {
		int y = 0;
		
		for (y = 0; y < 240; y++) {
			plot_pixel(x, y, 0);
		}
	}
}

//waits for the S value in the registry to change to 0 indication the frame is drawn
void wait_for_vsync(){
	volatile int *pixel_ctrl_ptr = (int*)0xFF203020;
	register int status;
	
	*pixel_ctrl_ptr = 1;
	
	status = *(pixel_ctrl_ptr + 3);
	while((status & 0x01) != 0){
		status = *(pixel_ctrl_ptr +3);
	}
}
	
	

int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
	volatile int * pixel_buffer_back_ptr = (int *) 0xFF203024;
    /* Read location of the pixel buffer from the pixel buffer controller */
    
	*pixel_buffer_back_ptr = *pixel_ctrl_ptr;
	
	
	wait_for_vsync();
	pixel_buffer_start = *pixel_ctrl_ptr;
	
    clear_screen();
	int y = 0;
	int true = 1;
	int up = 0;
	while(true){
		//draws horizontal line
		draw_line(0, y, 319, y, 0x001F);   // this line is blue
		
		//checks if line is at the top of bottom and erases last line
		if (y == 0){
			draw_line(0, (1), 319, (1), 0); 
			up = 0;
		}
		else if (y == 239){
			draw_line(0, 238, 319, 238, 0);
			up = 1;
		}
		//checks if line is moving up or done and erases last line
		else if (up == 0){
			draw_line(0, (y -1), 319, (y -1), 0); 
		}
		else if (up == 1){
			draw_line(0, (y + 1), 319, (y + 1), 0); 
		}
		
		//increments y depending on direction
		if(up){
			y--;
		}
		else{
			y++;
		}
		
		//used for delay
		wait_for_vsync();
	}
}