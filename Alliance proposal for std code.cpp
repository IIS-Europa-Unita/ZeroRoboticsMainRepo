//Begin page General
class General{
public:
    static bool imBlue;         //which shphere we are
    static float pos[3];
    static float myState[12];
    static float ourZone[4];    //assembly zones
    static int id;
    static float target[3];
    static float theirZone[3];
    static float otState[12];
    static float dockDist[3]};   //L|M|S, TODO
    
    static void copyVec(float *src,float *dest,int inPos,int dim){
        for(int i = inPos;i < dim;i++)
            dest[i]=src[i];
    }
    static void assign(float vec[], float x, float y, float z){
        vec[0]=x;
        vec[1]=y;
        vec[2]=z;
    }
    static float dist(float pos1[], float pos2[]){
        float tempVec[3];
        mathVecSubtract(tempVec,pos1,pos2,3);
        return mathVecMagnitude(tempVec,3);
    }
    static bool compare(float a,float b,float appr){
        return a-b <= appr;
    }
    static bool compareVec(float a[], float b[], float appr){
        return a[0]<b[0]+appr && a[0]>b[0]-appr &&
               a[1]<b[1]+appr && a[1]>b[1]-appr &&
               a[2]<b[2]+appr && a[2]>b[2]-appr;
    }
};
//End page General
//Begin page main
//proposal for std code



void init(){
    api.getMyZRState(General::myState);
    api.getOtherZRState(General::otState);
    game.dropSPS();//1st point to build the triangle
    General::imBlue=General::myState[1]>0;
    drawTriangle(General::imBlue);
}

void loop(){
    api.getMyZRState(General::myState);
    if(game.getNumSPSHeld()>0){//if you still have some SPSs
        drawTriangle(General::imBlue);
        DEBUG(("SPS draw triangle"));
    }
    else{
        game.getZone(General::ourZone);
        General::assign(General::theirZone,-General::ourZone[0],-General::ourZone[1],-General::ourZone[2]);
        
        game.getItemLoc(General::target,objSel.getID());
        move.setDestination(General::target,ITEM);//SPS just for now, then ITEM or FLYAROUND
    }

    move.everySec();
}

//End page main
//Begin page movement
//movement modes
#define SPS 0
#define ITEM 1
#define GOAROUND 2


class {//movement
    float target [3];   //the point that has to be reached
    int mode;           //how will it go (setPosTarget/forces/ecc...)
    float startDist;
    float curDist;
public:
    void everySec(){
        switch(mode){
            case 0:
                api.setPositionTarget(target);
                break;
            case 1:
                forces();
                break;
            case 2:
                flyaround();
                break;
        }
    }
    void setDestination(float dest[],int modality){
        getDestination(dest);
        mode=modality;
    }
    void getDestination(float dest[]){
        General::copyVec(dest,target,0,3);
        startDis = General::dist(myState,target);
    }
    void forces(){
        float forceV[3];
        General::assign(forceV,target[0]-myState[0],target[1]-myState[1],target[2]-myState[2]);
        curDist = General::dist(myState,targ_absolute);
        
        DEBUG(("curDis is %f \n", curDist));
        if (curDist>=0.009)
        {
            if (curDist>startDist)
                api.setForces(forceV);
            else
                api.setPositionTarget(target);
        }
        else
        {
            DEBUG(("arrived"));
        }
    }
    
    void flyaround(){
        float itemState[12];
        game.getItemZRState(itemState,ID);
        float itemAtt[3];
    
        for(int i =0; i<3; ++i)
            itemAtt[i] = itemState[6+i];
    
        float dist[3];
        float max=-1;
        int maxNum=-1;
        for(int i = 0; i<3; ++i){
            dist[i] = fabsf(General::myState[i]-itemState[i]);
            if (dist[i]>max)
            {
                max = dist[i];
                maxNum = i;
            }
        }
    
        float forcePoint[3];
        for(int i =0; i<3;++i)
            forcePoint[i] = itemAtt[i];
        forcePoint[maxNum]*=-1;
        forces();
        //counter++;
        //if (counter % 2 == 0) api.setForces(force_point);
    }
}move;
//End page movement
//Begin page rotate && triangle
void rotate(float target[3]){
    float tempVec[3];
    mathVecSubtract(tempVec, General::target, General::pos, 3);
    mathVecNormalize(tempVec, 3);
    api.setAttitudeTarget(tempVec);
}

void drawTriangle(bool imBlue){
    float target[3];
    if(game.getNumSPSHeld()>=2){// 2nd point
        DEBUG(("select target1"));
        if(imBlue)
            General::assign(General::target, 0.6, -0.6, 0.0);
        else
            General::assign(General::target, -0.6, 0.6, 0.0);
    }
    else if (game.getNumSPSHeld()>=1){ //last point
        DEBUG(("select target2"));
        if(General::imBlue)
            General::assign(target, 0.6, 0.5, 0.5);
        else
            General::assign(target, 0.6, -0.5, -0.5);
    }
    move.setDestination(General::target,SPS);
    DEBUG(("move %f",General::dist(General::target,General::myState) ));
    //if close enough to the point, drop it
    if(General::dist(General::target,General::myState)<0.08){
        game.dropSPS();
        DEBUG(("drop!"));
    }
    
}

//End page rotate && triangle
//Begin page select object
class {
    int itemChosen;
    int objID;
    float objIDPos[3], objIDp1Pos[3]; //ObjIDPos = pos of objID; objIDp1Pos = pos of objID+1
    float myState[12];
    float otState[12];
    bool isNearer(float objPos[]) {
        api.getMyZRState(myState);
        api.getOtherZRState(General::otState);
        if (!(game.hasItem(objID) != 2 || game.itemInZone(objID))) //Is obj free? 
            if (General::dist(myState, objPos) > General::dist(General::otState, objPos)) //Am i further than other?
                return false;   //I am further
            else
                return true;    //I am nearer
        else
            return false;       //Obj isn't free
    }        
public:
    int getID(){
        itemChosen = -1; 
        objID=0;
        do {
            game.getItemLoc(objIDPos, objID);
            game.getItemLoc(objIDp1Pos, objID+1);
            
            if (!isNearer(objIDPos))
                if (!isNearer(objIDp1Pos))
                    objID+=2;
                else
                    itemChosen = objID+1;
            else
                itemChosen = objID;
                
        } while(itemChosen == -1);
        
        return itemChosen;
    }
}objSel;
//End page select object
