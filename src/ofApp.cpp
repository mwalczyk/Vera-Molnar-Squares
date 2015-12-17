#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    // We draw everything into an FBO that is twice the size of the current window
    int border = 80;
    int size = ofGetWidth() * 2 - border;
    int sizeStep = size / NUM_SQUARES;
    ofPoint windowCenter(ofGetWidth(), ofGetHeight());
    
    for (int i = 0; i < NUM_SQUARES; ++i)
    {
        ofPolyline line;
        ofPoint upperLeft(windowCenter.x - size/2, windowCenter.y - size/2);
        ofPoint upperRight(windowCenter.x + size/2, windowCenter.y - size/2);
        ofPoint lowerRight(windowCenter.x + size/2, windowCenter.y + size/2);
        ofPoint lowerLeft(windowCenter.x - size/2, windowCenter.y + size/2);
        
        // This seqeunce of operations essentially builds a sub-divided square (polyline)
        line.addVertex(upperLeft);                                          // first, add the upper left point
        addPointBetween(upperLeft, upperRight, MAX_SUBDIVISIONS, line);
        line.addVertex(upperRight);                                         // then, add the upper right
        addPointBetween(upperRight, lowerRight, MAX_SUBDIVISIONS, line);
        line.addVertex(lowerRight);                                         // then, add the lower right
        addPointBetween(lowerRight, lowerLeft, MAX_SUBDIVISIONS, line);
        line.addVertex(lowerLeft);                                          // finally, add the lower left
        addPointBetween(lowerLeft, upperLeft, MAX_SUBDIVISIONS, line);
        line.close();
        
        mSquares.push_back(line);
        size -= sizeStep;
    }
    
    mSquareColor.set(65);
    ofSetLineWidth(3);
    
    // Allocate and clear our FBO
    mSquareFbo.allocate(ofGetWidth() * 2, ofGetHeight() * 2, GL_RGB, 8);
    mSquareFbo.begin();
    ofClear(ofColor::white);
    mSquareFbo.end();
    
    mPaperTexture.load("paper.jpg");
}

//--------------------------------------------------------------
void ofApp::addPointBetween(ofPoint &a, ofPoint &b, int numToAdd, ofPolyline &polyLine)
{
    // This function generates vertices between two points and adds them to the polyline
    float step = 1.0f / numToAdd;
    float pct = step;
    for (int i = 0; i < numToAdd; ++i)
    {
        polyLine.addVertex(a.getInterpolated(b, pct));
        pct += step;
    }
}

//--------------------------------------------------------------
void ofApp::update()
{
}

//--------------------------------------------------------------
void ofApp::draw()
{
    
    
    // EXAMPLE ANIMATION
    MAX_OFFSET = ofMap(sin(ofGetElapsedTimef() * 5), -1, 1, 0, 8);
    MAX_TRANSLATION = ofMap(sin(ofGetElapsedTimef() * 2 + 40), -1, 1, 0, 10);
    PCT_JITTER = ofMap(sin(ofGetElapsedTimef() * 0.5 + 20), -1, 1, 0, 1);
    
    
    
    //-------------------------------------------------------------- FBO begins
    mSquareFbo.begin();
    ofClear(ofColor::white);
    
    // Draw the background texture, if it's available
    if (mPaperTexture.isAllocated())
    {
        ofSetColor(ofColor::white);
        mPaperTexture.draw(0, 0, ofGetWidth() * 2, ofGetHeight() * 2);
    }
    ofSeedRandom(0);
    ofSetColor(mSquareColor);
    for (const auto &square: mSquares)
    {
        ofPolyline shiftedLine;
        for (auto &point: square.getVertices())
        {
            // Leave some of the points in their original positions
            if (ofRandomf() > PCT_JITTER)
            {
                shiftedLine.addVertex(point);
            }
            else {
                shiftedLine.addVertex(point.x + ofRandom(-MAX_OFFSET, MAX_OFFSET),
                                      point.y + ofRandom(-MAX_OFFSET, MAX_OFFSET));
            }
        }
        shiftedLine.close();
        
        ofPushMatrix();
        ofTranslate(ofRandom(-MAX_TRANSLATION, MAX_TRANSLATION),
                    ofRandom(-MAX_TRANSLATION, MAX_TRANSLATION));
        shiftedLine.draw();
        ofPopMatrix();
    }
    
    mSquareFbo.end();
    //-------------------------------------------------------------- FBO ends
    
    // Draw the FBO at 1/2 res, which equates to 504x504
    mSquareFbo.draw(0, 0, ofGetWidth(), ofGetHeight());
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if (key == 's' || key == 'S')
    {
        ofSaveScreen("screenshots/molnar_" + ofGetTimestampString() + ".jpg");
    }
}



