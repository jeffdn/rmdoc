/* test.c
 * for testing weird comments, to
 * see if they are properly removed.
 */

int main (int argc, char * /* blar */ argv[]) {
	// funky fresh
	/* I enjoy eating *******
	 * many things a day ****
	 * if you do not let me *
	 * I will call you gay **
	 ***********************/

	printf ("don't delete this /* documentation */ \" /*partay!*/ \"\n");
	
	return 0; // all done
}
