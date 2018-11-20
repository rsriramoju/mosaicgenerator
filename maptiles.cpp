/**
 * @file maptiles.cpp
 * Code for the maptiles function.
 */

#include <iostream>
#include <map>
#include "maptiles.h"
//#include "cs225/RGB_HSL.h"

using namespace std;
/**
*  This function puts together all the pieces of the
*  mosaic creator by taking in the tileimages and
*  the source image to find the closest 'tile' to
*  represent a group of pixels in the source image.
**/

Point<3> convertToXYZ(LUVAPixel pixel) {
    return Point<3>( pixel.l, pixel.u, pixel.v );
}

MosaicCanvas* mapTiles(SourceImage const& theSource,
                       vector<TileImage>& theTiles)
{
    map<Point<3>, TileImage*> imagemap;

    vector<Point<3>> points;
    for(unsigned k = 0; k < theTiles.size(); k++){
      LUVAPixel temp = theTiles[k].getAverageColor();
      Point<3> toins = convertToXYZ(temp);
      imagemap.insert(make_pair(toins, &(theTiles[k])));
      points.push_back(toins);
    }

    int rows = theSource.getRows();
    int cols = theSource.getColumns();
    MosaicCanvas *ret = new MosaicCanvas(rows, cols);

    KDTree<3> closest(points);
    for(int i = 0; i < rows; i++){
      for(int j = 0; j < cols; j++){
        LUVAPixel region = theSource.getRegionColor(i, j);
        Point<3> regionpoint = convertToXYZ(region);
        Point<3> inspoint = closest.findNearestNeighbor(regionpoint);

        for(auto it = imagemap.begin(); it != imagemap.end(); it++){
          if(inspoint == it->first){
            ret->setTile(i, j, it->second);
          }
        }
      }
    }
    return ret;
}
