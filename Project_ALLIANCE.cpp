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
    game.getItemZRState(itemState, num);
    float target[3];
    getMyPos();
    game.getItemLoc(target, num);
    ranking[num] = 1/(((dist(myPos, target)*dist(myPos, target))));
    switch(num){
        case 0:   //large
        case 1:
            ranking[num]*=2.5;
            break;
        case 2:   //medium
        case 3:
            ranking[num]*=1.5;
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
            ranking[num]*=0;
            break;
    }
    if(dist(target, theirPos) < 0.1 && packInTheirZone(num))
        ranking[num]*=0;
    game.getItemZRState(itemState, num);
    if(itemState[3] != 0.00 || itemState[4] != 0.00 || itemState[5] != 0.00){
        ranking[num]*=0;
        //DEBUG(("BLYAT CYKA PUTIN KURWA"));
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
    api.getMyZRState(myPos);
}

/*we get our position*/

void getTheirPos(){
    api.getOtherZRState(theirPos);
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
float   theirPos[3];
float   itemState[12];          //state of the item
float   itemAtt[3];             //attitude of the item
float   pointAtt[3];            //point attitude
float   second[3];              //point to place SPS
float   third[3];
float   dropPos[3];
float   dropZone[3];
float   temp[3];

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
int     packsInZone;

void init(){
    //DEBUG(("CE REPIGLIAMMO TUTTO CHILLO CH'ERA 'O NUOSTRO!"));
    packsInZone = 0;
    getMyPos();
    getTheirPos();
    index = 's';                //index starts here
    game.dropSPS();             //we drop the first SPS at our starting point
    calculated = false;
    if(ourColor() == 'B'){
        assign(second, 0.0, 0.15, 0.60);
        assign(third, 0.60, 0.0, 0.60);
    }
    else{
        assign(second, 0.0, 0.0, -0.60);
        assign(third, -0.60, 0.0, -0.60);
    }
    setDist();
}

void loop(){
    getMyPos();                            //we get our position because we always need that information
    packsInZone = 0;
    for(int i = 0; i < 4; i++){
        game.getItemLoc(temp, i);
        if(compareVector(ourZone, temp, 0.05))  //count num of packs in our zone
            packsInZone++;
        }
    if((game.getCurrentTime() >= 140 || packsInZone == 2) && index == 'p' ) //if time is low or we have 2 packs we go def
        index = 'f';
    switch(index){
        /*we call worthyPack to see what is the worthiest pack to pick up. If we didn't place the SPS we will go to case F and place it, 
        otherwise we will go for packs. we calculate here the virtual point or we would follow the pack if it starts moving*/
        case 's':
            if(!compareVector(myPos, second, 0.10))
                api.setPositionTarget(second);
            else{
                game.dropSPS();
                index = '3';
            }
            break;
        /*we calculate the position of the second SPS based on the position of the first worthyPack we find*/
        case '3':
            if(!compareVector(myPos, third, 0.10))
                api.setPositionTarget(third);
            else{
                game.dropSPS();
                zoneInfo();
                index = 'p';
            }
            break;
        /* */
        case 'p':
            worthyPack();
            if(!calculated)
                calcPoint();
            else{
                DEBUG(("GETTING PACK %d", targetNumber));
                approachPack();
                api.setAttitudeTarget(pointAtt);
                DEBUG(("DIST FROM TARGET: %f", dist(myPos, actualTarget)));
                if(dist(myPos, actualTarget)<=distMax && dist(myPos, actualTarget) >= distMin && game.isFacingCorrectItemSide(targetNumber)){
                    if(game.dockItem(targetNumber) && game.hasItem(targetNumber) == 1){
                        game.dropSPS();
                        assign(dropPos, myPos[0], myPos[1], myPos[2]);
                        index = 'z';
                    }
                }
            }
            break;
        /*we calculate the approach point. if the pack is moving we return to worthyPack, otherwise we go and dock the item. The first
        dock will place the last SPS and get the ZoneInfo*/
        case 'z':
            DEBUG(("RUSH B"));
            rotateToPoint(ourZone);
                api.setPositionTarget(ourZone);
            if((dist(dropPos, ourZone) < 0.2))
                assign(dropZone, ourZone[0], ourZone[1], ourZone[2] - 0.2);
            if(packInZone()){
                game.dropItem();
                index = 'p';
                calculated = false;
            }
            break;
        /*we drop the item in the zone. the first time we drop the item and calculate an universal dropping point*/
        case 'f':
            //DEBUG(("VUO FA' CODICE E CODICE? PO PO PO"));
            api.setPositionTarget(ourZone);
            break;
    }
}
//End page main
