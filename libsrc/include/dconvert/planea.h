#ifndef __Header_planea__
#define __Header_planea__

#include "plane.h"

class ImagePlaneLister {
ImagePlane *plane;
public:
	ImagePlaneLister(ImagePlane& p)
		{
			plane=&p;
		}

	void addPlaneToList(AttributeList& list);
};

#endif /* __Header_planea__ */
