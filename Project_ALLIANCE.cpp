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
//Begin page Movement
void rotateToPoint(float target[3]){
    float temp[3];
    getMyPos();
    mathVecSubtract(temp, target, myPos, 3);
    mathVecNormalize(temp, 3);
    api.setAttitudeTarget(temp);
}

/*rotates to a point*/

void moveTo(float* target){
    float between[3];
    for(int i =0; i<3; ++i){
        between[i] = target[i] - myState[i];
    }
    
    float curDis = dist(myState, target);
    
    if (curDis >= 0.1){ // distance on what it really shouldnt apply forces
        if (counter % 4 == 0) api.setForces(between); //frequency of applying forces. frequency is more when number is less 
        counter++;
    }
    api.setPositionTarget(target);
}


void goAround(int targetNumber){
    game.getItemZRState(itemState, targetNumber);
    float itemAtt[3], dist[3], b[3], c[3], forcePoint[3];
    float max=-1;
    int max_num=-1;
    for(int i =0; i<3; ++i)
        itemAtt[i] = itemState[6+i];
    for(int i = 0; i<3; ++i){
        dist[i] = fabsf(myState[i]-itemState[i]);
        if (dist[i]>max){
            max = dist[i];
            max_num = i;
        }
    }
    for(int i =0; i<3;++i)
        forcePoint[i] = itemAtt[i];
    forcePoint[max_num]*=-1;
    mathVecAdd(c, forcePoint, itemAtt, 3);
    mathVecNormalize(c, 3);
    for(int i = 0; i < 3; ++ i)
        c[i]/=5;
    mathVecAdd(b, virtualTarget, c, 3);
    moveTo(b);
}

void approachPack(){
    float x[3];
    float a, b, c;
    for(int i = 0; i < 3; ++i)
        x[i] = itemAtt[i] + actualTarget[i];
    if(!compareVector(myPos, virtualTarget, 0.005)){
        a = dist(myPos, virtualTarget);
        b = dist(x, virtualTarget);
        c = dist(myPos, x);
        if(a*a + b*b < c*c)
            goAround(targetNumber);
        else
            moveTo(virtualTarget);
    }
}
//End page Movement
//Begin page Packs
void worthyPack() {
    for(int i = 0; i < 4; i++) {
        getRank(i);
    }
    getWorthyPackInfo();
    setDist();
}

/*cumulative function to learn which pack is the worthiest*/


void getRank(int num) {
    game.getItemZRState(itemState, num);
    float target[3];
    getMyPos();
    game.getItemLoc(target, num);
    ranking[num] = 1/(((dist(myPos, target)*dist(myPos, target))));
    if(num == 0 || num == 1)
            ranking[num]*= 1.7;
    if(game.itemInZone(num))
        ranking[num] = NULL;
    game.getItemZRState(itemState, num);
    if(packIsMoving(num)){
        ranking[num] = NULL;
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
            distFromTarget = .162;
            distMin = 0.151;
            distMax = 0.173;
            break;
        case 2:
        case 3:
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
    game.getZone(zoneData);
    assign(ourZone, zoneData[0], zoneData[1], zoneData[2]);
    assign(theirZone, zoneData[0]*(-1), zoneData[1]*(-1), zoneData[2]*(-1));
}

/*we get the location of our and their zone*/


bool packIsMoving(int id){
    game.getItemZRState(itemState, id);
    return(itemState[3] != 0.00 || itemState[4] != 0.00 || itemState[5] != 0.00);
}

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


bool packInZone(){
    float temp[3];
    game.getItemLoc(temp, targetNumber);
    if(dist(temp, ourZone) < 0.22 - zoneData[3])
        return true;
    else 
        return false; 
}

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
//End page Position
//Begin page main
float   myState[12];            //status of the sphere
float   myPos[3];               //our position
float   itemState[12];          //state of the item
float   itemAtt[3];             //attitude of the item
float   pointAtt[3];            //point attitude
float   sps[3];
float   zoneData[4];

float   virtualTarget[3];       //we calculate and fly to this point
float   actualTarget[3];        //actual location of an item
float   distFromTarget;         //distance from our target
int     targetNumber;           //ID of the item
float   distMin;                //minimum distance of docking
float   distMax;                //maximum distance of docking

float   ranking[4];             //vector that is used to calculate which item is the worthiest

float   ourZone[3];             //our assembly zone
float   theirZone[3];           //they assembly zone

char    index;                  //switch index
bool    calculated;             //check if the virtualPoint is calculated
int     counter;                //counter used to fly around objects
int     packsInZone;            //number of packs into our zone
bool    first;

void init(){
    getMyPos();
    index = 's';                //index starts here
    game.dropSPS();             //we drop the first SPS at our starting point
    calculated = false;
    if(ourColor() == 'B'){
        assign(sps, -0.40, 0.40, 0.0);
    }
    else{
        assign(sps, 0.40, -0.40, 0.0);
    }
    first = true;
    setDist();
}

void loop(){
    getMyPos();                            //we get our position because we always need that information
    packsInZone = 0;
    for(int i = 0; i < 4; i++){
        if(game.itemInZone(i))              //count num of packs in our zone
            packsInZone++;
    }
    
    if((game.getCurrentTime() >= 155 || packsInZone == 2) && index == 'p' ) //if time is low or we have 2 packs we go def
        index = 'f'; 
        
    if(packIsMoving(targetNumber) &&  game.getNumSPSHeld() == 0 && index != 'z')
        index = 'p';
    if(game.hasItem(targetNumber) == 2)
        index = 'p';
        switch(index){
        /*we call worthyPack to see what is the worthiest pack to pick up. If we didn't place the SPS we will go to case F and place it, 
        otherwise we will go for packs. we calculate here the virtual point or we would follow the pack if it starts moving*/
        case 's':
            if(!compareVector(myPos, sps, 0.10))
                moveTo(sps);
            else{
                game.dropSPS();
                index = 'p';
            }
            break;
        /*we calculate the position of the second SPS based on the position of the first worthyPack we find*/
        case 'p':
            DEBUG(("WORTHY PACK"));
            worthyPack();
            calcPoint();
            index = 'm';
            break;
        case 'm':
            DEBUG(("GETTING PACK %d", targetNumber));
            approachPack();
            api.setAttitudeTarget(pointAtt);
            DEBUG(("DIST FROM TARGET: %f", dist(myPos, actualTarget)));
            if(dist(myPos, actualTarget)<=distMax && dist(myPos, actualTarget) >= distMin && game.isFacingCorrectItemSide(targetNumber)){
                if(game.dockItem(targetNumber) && game.hasItem(targetNumber) == 1){
                    if(first){
                        game.dropSPS();
                        zoneInfo();
                        first = false;
                    }
                    index = 'z';
                }
            }
            break;
        /*we calculate the approach point. if the pack is moving we return to worthyPack, otherwise we go and dock the item. The first
        dock will place the last SPS and get the ZoneInfo*/
        case 'z':
            DEBUG(("GOING TO ZONE"));
            rotateToPoint(ourZone);
            moveTo(ourZone);
            if(packInZone()){
                game.dropItem();
                index = 'p';
                calculated = false;
            }
            break;
        /*we drop the item in the zone. the first time we drop the item and calculate an universal dropping point*/
        case 'f':
            moveTo(ourZone);
            break;
    }
}
//End page main
