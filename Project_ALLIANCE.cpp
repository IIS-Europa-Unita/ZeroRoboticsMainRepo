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
    getMyPos(myPos);
    mathVecSubtract(temp, target, myPos, 3);
    mathVecNormalize(temp, 3);
    api.setAttitudeTarget(temp);
}

void secondSPS(float targetNumber){
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

void setZonePoint(){
    game.getItemLoc(actualTarget, targetNumber);
    ourZonePos[0] = actualTarget[0];
    ourZonePos[1] = actualTarget[1];
    if(actualTarget[2]>0.1)
        ourZonePos[2] = actualTarget[2] - 0.2;
    else
        ourZonePos[2] = actualTarget[2] + 0.2;
}
//End page Move
//Begin page Packs
void worthyPack() {
    for(int i = 0; i < 4; i++) {
        getRank(i);
    }
    getWorthyPackInfo();
    setDist();
}

int itemStat(int num) {
    if(game.itemInZone(num))
        return 3;
        
    if(packInTheirZone(num) && game.hasItem(num) == 0)
        return 4;
        
    return(game.hasItem(num));
}

void getRank(int num) {
    float target[3];
    getMyPos(myPos);
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

void setDist(){
    switch(targetNumber){
        case 0:
        case 1:
            distFromTarget = .182; 
            distMin = 0.151;
            distMax = 0.173;
            break;
        case 2:
        case 3:
            distFromTarget = .1725;
            distMin = 0.138;
            distMax = 0.160;
            break;
    }
}
//End page Packs
//Begin page Position
char ourColor(){
    getMyPos(myPos);
    if(myPos[1] > 0)
        return 'B';
    else
        return 'R';
}

void zoneInfo(){
    float zoneData[4];
    game.getZone(zoneData);
    assign(ourZone, zoneData[0], zoneData[1], zoneData[2]);
    assign(theirZone, zoneData[0]*(-1), zoneData[1]*(-1), zoneData[2]*(-1));
}

bool packInZone(){
    float temp[3];
    game.getItemLoc(temp, targetNumber);
    if(compareVector(temp, ourZone, 0.05))
        return true;
    else 
        return false; 
}

bool packInTheirZone(int id){
    float temp[3];
    game.getItemLoc(temp, id);
    if(compareVector(temp, theirZone, 0.08))
        return true;
    else
        return false;
}

void getMyPos(float x[3]) {
    api.getMyZRState(myState);
    copyArray(myState, x, 0, 3);
}

void getItemAtt(float x[3], int targetNumber) {
    game.getItemZRState(itemState, targetNumber);
    copyArray(itemState, x, 4, 3);
}
//End page Position
//Begin page approach
void calcPoint(int targetNumber){
    game.getItemZRState(itemState, targetNumber);
    game.getItemLoc(actualTarget, targetNumber);
    for(int i=0; i<3; i++){
        itemAtt[i] = itemState[i+6];
        pointAtt[i] = -itemAtt[i];
    }
    mathVecNormalize(itemAtt, 3);
    float length = mathVecMagnitude(itemAtt, 3);
    for(int i=0; i<3; i++)
        virtualTarget[i] = itemAtt[i] * dockDist[targetNumber/2]/length + actualTarget[i];
}

void approachPack(int targetNumber){
    if(!compareVector(myPos, virtualTarget, 0.005)){
        float dis_to_point_near = dist(myState, virtualTarget);
        float dis_to_center = dist(myState, actualTarget);
        if ((dis_to_point_near>dis_to_center) && (dis_to_point_near<=0.4)){
            goAround(targetNumber);
        }
            else{
            api.setPositionTarget(virtualTarget);
        }
    }
}


void goAround(int targetNumber){
    float dist[3];
    float max=-1;
    int max_num=-1;
    for(int i = 0; i<3; ++i)
    {
        dist[i] = fabsf(myState[i]-itemState[i]);
        if (dist[i]>max)
        {
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
//End page approach
//Begin page main
float   myState[12];    //status
float   myPos[3];
float   itemState[12];
float   itemAtt[3];
float   pointAtt[3];
float   dockDist[6];
float   sps[3];

float   virtualTarget[3];  //target information
float   actualTarget[3];
float   distFromTarget;
int     targetNumber;
float   distMin;
float   distMax;

float   ranking[4];

float   ourZone[3];     //assembly zones
float   ourZonePos[3];
float   theirZone[3];

char    index;    //switch index
bool    check;
bool    checkZone;

int     counter;

void init(){
    dockDist[0] = 0.173;
	dockDist[1] = 0.160;
    dockDist[2] = 0.146;
    index = 'w';
    game.dropSPS();
    check = true;
    checkZone = true;
}

void loop(){
    getMyPos(myPos);
    DEBUG(("%d", targetNumber));
    switch(index){
        case 'w':
            if(!checkZone){
                    setZonePoint();
                    checkZone = false;
            }
            worthyPack();
            if(check)
                index = 'f';
            else{
                index = 'p';
                calcPoint(targetNumber);
            }
            break;
        case 'f':
            secondSPS(targetNumber);
            if(!compareVector(myPos, sps, 0.15))
                api.setPositionTarget(sps);
            else{
                game.dropSPS();
                index = 'p';
            }
            break;
        case 'p':
            if(check)
                calcPoint(targetNumber);
            approachPack(targetNumber);
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
            break;
        case 'z':
            rotateToPoint(ourZone);
            api.setPositionTarget(ourZone);
            if(packInZone()){
                game.dropItem();
                index = 'w';
            }
            break;
    }
}
//End page main
