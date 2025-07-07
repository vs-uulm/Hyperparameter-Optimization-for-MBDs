//
// LinkControl - models Links that block radio transmissions
// Copyright (C) 2010 Christoph Sommer <christoph.sommer@informatik.uni-erlangen.de>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#include "LinkControl.h"
#include "F2MDParameters.h"
#include "Coord.h"
#include "Link.h"

LinkControl::~LinkControl() {

}



double LinkControl::calculateDistance(const veins::Coord& pos, double deltaX, double deltaY) {
    // calculate bounding box of transmission
    veins::Coord bboxP1 = veins::Coord(std::min(pos.x - deltaX, pos.x + deltaX),
            std::min(pos.y - deltaY, pos.y + deltaY));
    veins::Coord bboxP2 = veins::Coord(std::max(pos.x - deltaX, pos.x + deltaX),
            std::max(pos.y - deltaY, pos.y + deltaY));

    size_t fromRow = std::max(0, int(bboxP1.x / GRIDCELL_SIZE));
    size_t toRow = std::max(0, int(bboxP2.x / GRIDCELL_SIZE));
    size_t fromCol = std::max(0, int(bboxP1.y / GRIDCELL_SIZE));
    size_t toCol = std::max(0, int(bboxP2.y / GRIDCELL_SIZE));

    if (fromRow > 0) {
        fromRow = fromRow - 1;
    }
    if (fromCol > 0) {
        fromCol = fromCol - 1;
    }
    toRow = toRow + 1;
    toCol = toCol + 1;

    std::set<Link*> processedLinks;

    double distance = DBL_MAX;
    double localD = 0;
    for (size_t col = fromCol; col <= toCol; ++col) {
        if (col >= Links.size())
            break;
        for (size_t row = fromRow; row <= toRow; ++row) {
            if (row >= Links[col].size())
                break;
            const LinkGridCell& cell = (Links[col])[row];
            for (LinkGridCell::const_iterator k = cell.begin();
                    k != cell.end(); ++k) {

                Link* o = *k;
                localD = o->getDistance(&pos);
                if(localD<distance){
                    distance = localD;
                    if(localD<params->MAX_DISTANCE_FROM_ROUTE){
                        return localD;
                    }
                }
                    }
        }
    }
    return distance;
}



