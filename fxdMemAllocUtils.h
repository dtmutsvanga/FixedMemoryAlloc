#ifndef FXDMEMALLOCUTILS_H_
#define FXDMEMALLOCUTILS_H_

#define FMAUTIL_MAX(x, y) 	((x) > (y) ? (x) : (y))  			/* Get maximum. Avoid creating block size less than pointer size */
#define FMAUTIL_ALIGN(x, y)	((x)%(y)==0? (x) : ((x) + (y)) - ((x)%(y)))			/* Align x to y */
#define FMAUTIL_ASSERT(x)   {do {} while((x) == 0);}             /* Assert */
#endif //FXDMEMALLOCUTILS_H_
