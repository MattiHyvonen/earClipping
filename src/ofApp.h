#pragma once

#include "ofMain.h"
#include <vector>


struct triangle{
    triangle(){}
    triangle(ofVec2f A, ofVec2f B, ofVec2f C);
    ofVec2f points[3];
    bool isInside(ofVec2f P);
};


class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
                bool vertexIsConvex(int i);
                void triangulate();
                void poistaKorva();
                
		void draw();
		void drawPoints();
                void drawPolygon();
                void drawTriangle(triangle);
                
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
                
                std::vector<ofVec2f> points;
                std::vector<triangle> triangles;
                triangle T;
                ofVec2f middle;
                ofVec2f P;
};
