/*********************************************************************
 * RobWork Version 0.2
 * Copyright (C) Robotics Group, Maersk Institute, University of Southern
 * Denmark.
 *
 * RobWork can be used, modified and redistributed freely.
 * RobWork is distributed WITHOUT ANY WARRANTY; including the implied
 * warranty of merchantability, fitness for a particular purpose and
 * guarantee of future releases, maintenance and bug fixes. The authors
 * has no responsibility of continuous development, maintenance, support
 * and insurance of backwards capability in the future.
 *
 * Notice that RobWork uses 3rd party software for which the RobWork
 * license does not apply. Consult the packages in the ext/ directory
 * for detailed information about these packages.
 *********************************************************************/

#include "RenderSTL.hpp"

#include <rw/geometry/Geometry.hpp>
#include <rw/geometry/GeometrySTL.hpp>

#include <rw/math/Vector3D.hpp>

using namespace rwlibs::drawable;
using namespace rw::geometry;

using namespace rw::geometry;
using namespace rwlibs::drawable;

namespace {

/*	class FaceArrayGeometry: public Geometry {
	private:
		std::list<Face<float> >& _faces;
	public:
		FaceArrayGeometry(std::list<Face<float> >& faces):
			_faces(faces)
		{
		}
		
		const std::list<Face<float> >& getFaces() const {
			return _faces;
		}
	};
	*/

    void drawFace(const Face<float>& face)
    {
        glNormal3fv(face._normal);
        glVertex3fv(face._vertex1);
        glVertex3fv(face._vertex2);
        glVertex3fv(face._vertex3);
    }
    
    void setArray4(float *array, float v0, float v1, float v2, float v3 ){
    	array[0]=v0;
    	array[1]=v1;
    	array[2]=v2;
    	array[3]=v3;
    }
}

RenderSTL::RenderSTL(const std::string &filename):
	_r(0.8),_g(0.8),_b(0.8)
{
	GeometrySTL::ReadSTL(filename, _faces);
	//_renderer = new RenderGeometry( _id, new FaceArrayGeometry(_faces));
	setArray4(_diffuse, 0.8,0.8,0.8,1.0);
	setArray4(_ambient, 0.2,0.2,0.2,1.0);
	setArray4(_emission, 0.0,0.0,0.0,0.0);
	setArray4(_specular, 0.2,0.2,0.2,1.0);
	_shininess[0] = 128;

	// create displaylist
    _displayListId = glGenLists(1);
    glNewList(_displayListId, GL_COMPILE);
    glPushMatrix();
    glBegin(GL_TRIANGLES);
    // Draw all faces.
    // TODO: faces should have norma
    std::for_each(_faces.begin(), 
    		  	  _faces.end(), drawFace);
    glEnd();
    glPopMatrix();
    glEndList();
}

void RenderSTL::setFaces(const std::vector<Face<float> >& faces) {
    _faces = faces;
}

void RenderSTL::draw(DrawType type, double alpha) const{
	glColor4f(_r, _g, _b, alpha);
	_diffuse[3] = alpha;
	glMaterialfv(GL_FRONT, GL_AMBIENT, _ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, _diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, _specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, _shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, _emission);
	//glPushAttrib(GL_CURRENT_BIT);

	switch(type){
    case Render::SOLID:
    	glPolygonMode(GL_FRONT, GL_FILL);
		glCallList(_displayListId);
		break;
    case Render::OUTLINE: // Draw nice frame
    	glPolygonMode(GL_FRONT, GL_FILL);
		glCallList(_displayListId);
    case Render::WIRE: // Draw nice frame
    	glPolygonMode(GL_FRONT, GL_LINE);
    	glCallList(_displayListId);
    	break;
	}
}
