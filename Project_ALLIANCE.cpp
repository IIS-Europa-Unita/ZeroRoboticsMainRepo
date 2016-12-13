//Begin page Misc
void copyArray(float *src, float *dest, int inPos, int dim){
    for(int i = inPos;i < dim;i++)
        dest[i]=src[i];
}

bool compareVector(float a[], float b[], float approx){
    if  (((a[0] < (b[0]+approx))&&(a[0] > (b[0]-approx))) &&
        ((a[1] < (b[1]+approx))&&(a[1] > (b[1]-approx))) &&
        ((a[2] < (b[2]+approx))&&(a[2] > (b[2]-approx))))
        return true;
    else
        return false;
}

void assign(float vec[], float x, float y, float z){
    vec[0]=x;
    vec[1]=y;
    vec[2]=z;
}

float dist(float myPos[], float targetPos[]){
    float tempVec[3];
	mathVecSubtract(tempVec,targetPos,myPos,3);
	return (mathVecMagnitude(tempVec,3));
}


bool compare(float dist, float min, float max){
    if(dist >= min && dist <= max )
        return true;
    else
        return false;
}
//End page Misc
//Begin page Move
void rotateToPoint(float target[3]){
    float temp[3];
    getMyPos();
    mathVecSubtract(temp, target, myPos, 3);
    mathVecNormalize(temp, 3);
    api.setAttitudeTarget(temp);
}

/*rotates to a point*/

void secondSPS(){
    game.getItemZRState(itemState, targetNumber);
    game.getItemLoc(actualTarget, targetNumber);
    for(int i=0; i<3; i++){
        itemAtt[i] = itemState[i+6];
        pointAtt[i] = -itemAtt[i];
    }
    mathVecNormalize(itemAtt, 3);
    for(int i=0; i<3; i++)
        sps[i] = itemAtt[i] * 0.6 + actualTarget[i];
}

/*calculates the second SPS placing point based on pack attitude*/

void setZonePoint(){
    game.getItemLoc(actualTarget, targetNumber);
    ourZonePos[0] = actualTarget[0];
    ourZonePos[1] = actualTarget[1];
    if(actualTarget[2]>0.1)
        ourZonePos[2] = actualTarget[2] - 0.1;
    else
        ourZonePos[2] = actualTarget[2] + 0.1;
}

/*calculates the universal dropping point*/
//End page Move
//Begin page Packs
void worthyPack() {
    for(int i = 0; i < 4; i++) {
        getRank(i);
    }
    getWorthyPackInfo();
    setDist();
}

/*cumulative function to learn which pack is the worthiest*/

int itemStat(int num) {
    if(game.itemInZone(num))
        return 3;
        
    if(packInTheirZone(num) && game.hasItem(num) == 0)
        return 4;
        
    return(game.hasItem(num));
}

/* we check if the pack is in our or their zone to calculate the real rating of the item*/

void getRank(int num) {
    float target[3];
    getMyPos();
    game.getItemLoc(target, num);
    ranking[num] = 1/(dist(myPos, target));
    switch(num){
        case 0:   //large
        case 1:
            ranking[num]*=4.5;
            break;
        case 2:   //medium
        case 3:
            ranking[num]*=2.5;
            break;
    }
    switch(itemStat(num)){
        case 0:
            ranking[num]*=1;
            break;
        case 4:
            ranking[num]*=1.75;
            break;
        default:
            ranking[num]*=-100;
            break;
    }
}

/* the rating is based on distance and points/second that the item gives*/

void getWorthyPackInfo(){
    targetNumber = 0;
    float max = ranking[0];
    for(int i = 0; i < 4; i++){
        if(ranking[i] > max){
            targetNumber = i;
            max = ranking[i];
        }
    }
    game.getItemLoc(actualTarget, targetNumber);
}

/*calculates the max valued pack */

void setDist(){
    switch(targetNumber){
        case 0:
        case 1:
            //distFromTarget = .182;
            distFromTarget = .162;
            distMin = 0.151;
            distMax = 0.173;
            break;
        case 2:
        case 3:
            //distFromTarget = .1725;
            distFromTarget = .143;
            distMin = 0.138;
            distMax = 0.160;
            break;
    }
}

/*set docking distances based on pack. (probably useless with the new functions we're using) */
//End page Packs
//Begin page Position
char ourColor(){
    getMyPos();
    if(myPos[1] > 0)
        return 'B';
    else
        return 'R';
}

/*we learn our color. Is it relevant in alliance?*/

void zoneInfo(){
    float zoneData[4];
    game.getZone(zoneData);
    assign(ourZone, zoneData[0], zoneData[1], zoneData[2]);
    assign(theirZone, zoneData[0]*(-1), zoneData[1]*(-1), zoneData[2]*(-1));
}

/*we get the location of our and their zone*/

bool packInZone(){
    float temp[3];
    game.getItemLoc(temp, targetNumber);
    if(compareVector(temp, ourZone, 0.05))
        return true;
    else 
        return false; 
}

/*we check if a pack is in our zone or not*/

bool packInTheirZone(int id){
    float temp[3];
    game.getItemLoc(temp, id);
    if(compareVector(temp, theirZone, 0.08))
        return true;
    else
        return false;
}

/*we check if a pack is in their zone or not*/

void getMyPos() {
    api.getMyZRState(myState);
    copyArray(myState, myPos, 0, 3);
}

/*we get our position*/


bool checkSPSPos(float point[3]){
    if(point[0] > 0.64 || point[0] < -0.64 || point[1] > 0.80 || point[1] < -0.80 || point[2] > 0.64 || point[2] < -0.64 
    || (compareVector(startingPos, point, 0.2)))
        return true;
    else
        return false;
}
//End page Position
//Begin page approach
void calcPoint(){
    game.getItemZRState(itemState, targetNumber);
    if(itemState[3] <= 0.1 && itemState[4] <= 0.1 && itemState[5] <= 0.1){
        game.getItemLoc(actualTarget, targetNumber);
        for(int i=0; i<3; i++){
            itemAtt[i] = itemState[i+6];
            pointAtt[i] = -itemAtt[i];
        }
        mathVecNormalize(itemAtt, 3);
        float length = mathVecMagnitude(itemAtt, 3);
        for(int i=0; i<3; i++)
            virtualTarget[i] = itemAtt[i] * distFromTarget/length*0.99 + actualTarget[i];
        calculated = true;
    }
}

/*this function calculates the docking point based on the position, attitude and type of item*/

void approachPack(){
    if(!compareVector(myPos, virtualTarget, 0.005)){
        float dis_to_point_near = dist(myPos, virtualTarget);
        float dis_to_center = dist(myPos, actualTarget);
        if ((dis_to_point_near>dis_to_center) && (dis_to_point_near<=0.4)){
            goAround();
        }
            else{
            api.setPositionTarget(virtualTarget);
        }
    }
}


/*we go to the docking point*/

void goAround(){
    float dist[3];
    float max=-1;
    int max_num=-1;
    for(int i = 0; i<3; ++i){
        dist[i] = fabsf(myState[i]-itemState[i]);
        if (dist[i]>max){
            max = dist[i];
            max_num = i;
        }
    }
    
    float force_point[3];
    for(int i =0; i<3;++i)
        force_point[i] = itemAtt[i];
    force_point[max_num]*=-1;
    counter++;
    if (counter % 2 == 0) api.setForces(force_point);
}

/*this function let us fly around an item to avoid collision*/
//End page approach
//Begin page main
float   myState[12];            //status of the sphere
float   myPos[3];               //our position
float   startingPos[3];
float   itemState[12];          //state of the item
float   itemAtt[3];             //attitude of the item
float   pointAtt[3];            //point attitude
float   sps[3];                 //point to place SPS

float   virtualTarget[3];       //we calculate and fly to this point
float   actualTarget[3];        //actual location of an item
float   distFromTarget;         //distance from our target
short int     targetNumber;           //ID of the item
float   distMin;                //minimum distance of docking
float   distMax;                //maximum distance of docking
float   startDis;

float   ranking[4];             //vector that is used to calculate which item is the worthiest

float   ourZone[3];             //our assembly zone
float   ourZonePos[3];          //virtual point calculated to never fail the drop in zone
float   theirZone[3];           //they assembly zone

char    index;                  //switch index
bool    check;                  //check if SPS is placed
bool    checkZone;              //check if we have the first item in zone
bool    calculated;             //check if the virtualPoint is calculated
short int     counter;                //counter used to fly around objects

void init(){
    getMyPos();
    copyArray(myPos, startingPos, 0, 3);
    index = 's';                //index starts here
    game.dropSPS();             //we drop the first SPS at our starting point
    check = true;               //setting bools 
    checkZone = true;
    calculated = false;
    if(ourColor() == 'B')
        targetNumber = 0;
    else
        targetNumber = 1;
    setDist();
}

void loop(){
    getMyPos();                            //we get our position because we always need that information
    if((game.getFuelRemaining() <= 20 || game.getCurrentTime() >= 150) && index == 'w')
        index = 'f';
    switch(index){
        case 'w':
            if(!checkZone){
                    setZonePoint();
            }
            worthyPack();
            if(check)
                index = 's';
            else
                index = 'p';
            break;
        /*we call worthyPack to see what is the worthiest pack to pick up. If we didn't place the SPS we will go to case F and place it, 
        otherwise we will go for packs. we calculate here the virtual point or we would follow the pack if it starts moving*/
        case 's':
            if(checkSPSPos(sps)){
                if(ourColor() == 'B')
                    assign(sps, 0.0, -0.15, 0.55);
                else
                    assign(sps, 0.0, 0.15, 0.55);
            }
            if(!compareVector(myPos, sps, 0.10))
                api.setPositionTarget(sps);
            else{
                game.dropSPS();
                index = 'p';
            }
            break;
        /*we calculate the position of the second SPS based on the position of the first worthyPack we find*/
        case 'p':
            if(!calculated)
                calcPoint();
            else{
                approachPack();
                api.setAttitudeTarget(pointAtt);
                DEBUG(("%f", dist(myPos, actualTarget)));
                if(dist(myPos, actualTarget)<=distMax && dist(myPos, actualTarget) >= distMin && game.isFacingCorrectItemSide(targetNumber)){
                    if(game.dockItem(targetNumber) && game.hasItem(targetNumber) == 1){
                        if(check){
                            game.dropSPS();
                            zoneInfo();
                            check = false;
                        }
                        index = 'z';
                    }
                }
            }
            if(game.hasItem(targetNumber) == 2)
                index = 'w';
            break;
        /*we calculate the approach point. if the pack is moving we return to worthyPack, otherwise we go and dock the item. The first
        dock will place the last SPS and get the ZoneInfo*/
        case 'z':
            rotateToPoint(ourZone);
            if(checkZone)
                api.setPositionTarget(ourZone);
            else
                api.setPositionTarget(ourZonePos);
            if(packInZone()){
                game.dropItem();
                index = 'w';
                checkZone = false;
                calculated = false;
            }
            break;
        /*we drop the item in the zone. the first time we drop the item and calculate an universal dropping point*/
        case 'f':
            api.setPositionTarget(ourZone);
        break;
    }
}
//End page main
