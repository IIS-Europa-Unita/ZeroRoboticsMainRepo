//proposal for std code

#define s1 0.162
#define s2 0.149
#define s3 0.135 //still don't like those 3 consts

//movement modes
#define SPS 0
#define ITEM 1
#define GOAROUND 2

int sps; //game.getSPSRemaining() or similar
float cubeposes[6][3];

float pos[3];
float myState[12];
float att[3];
float cube_pos[3];
float s;


bool flag1, flag3;
bool flag;//flag...very explicative
bool team;
bool trianglend;
bool freep;
bool first;
bool indo;

int id;
float ps[3];
float point[3];


float   itemState[12];
float   itemAtt[3];
float   pointAtt[3];
float   dockDist[3];//L|M|S

float   actualTarget[3];
float   distFromTarget;
int     itemNumber;
float   distMin;
float   distMax;

float   ranking[4];

float   ourZone[4];     //assembly zones
float   theirZone[3];

char    index;    //switch index
bool    check;


bool imBlue;//which shphere we are
int phase;  //just to keep track of what do we have to do

class General{
public:
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
        //I needed this line to fold the function...lol
    }
    static bool compareVec(float a[], float b[], float appr){
        return a[0]<b[0]+appr && a[0]>b[0]-appr &&
               a[1]<b[1]+appr && a[1]>b[1]-appr &&
               a[2]<b[2]+appr && a[2]>b[2]-appr;
    }
};

class Movement{//movement
private:
    float target [3]; //the point that has to be reached
    int mode;//how will it go (setPosTarget/forces/ecc...)
    int ID;
    float status[12];
public:
    void everySec(int id, float state[]){
        ID=id;
        General::copyVec(state, status, 0, 12);
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
        General::copyVec(dest,target,0,3);
        mode=modality;
    }
    void getDestination(float dest[]){
        General::copyVec(target,dest,0,3);
    }
    void forces();
    void flyaround()
    {
        float itemState[12];
        game.getItemZRState(itemState,ID);
        float item_att[3];
    
        for(int i =0; i<3; ++i)
            item_att[i] = itemState[6+i];
    
        float dist[3];
        float max=-1;
        int max_num=-1;
        for(int i = 0; i<3; ++i){
            dist[i] = fabsf(status[i]-itemState[i]);
            if (dist[i]>max)
            {
                max = dist[i];
                max_num = i;
            }
        }
    
        float force_point[3];
        for(int i =0; i<3;++i)
            force_point[i] = item_att[i];
        force_point[max_num]*=-1;
        forces();
        //counter++;
        //if (counter % 2 == 0) api.setForces(force_point);
    }
}move;

void rotate(float target[3]){
    float tempVec[3];
    mathVecSubtract(tempVec, target, pos, 3);
    mathVecNormalize(tempVec, 3);
    api.setAttitudeTarget(tempVec);
}

void drawTriangle(bool imBlue){
    float target[3];
    if(game.getNumSPSHeld()>=2){// 2nd point
        DEBUG(("select target1"));
        if(imBlue)
            General::assign(target, 0.6, -0.6, 0.0);
        else
            General::assign(target, -0.6, 0.6, 0.0);
    }
    else if (game.getNumSPSHeld()>=1){ //last point
        DEBUG(("select target2"));
        if(imBlue)
            General::assign(target, 0.6, 0.5, 0.5);
        else
            General::assign(target, 0.6, -0.5, -0.5);
    }
    move.setDestination(target,SPS);
    DEBUG(("move %f",General::dist(target,myState) ));
    //if close enough to the point, drop it
    if(General::dist(target,myState)<0.08){
        game.dropSPS();
        DEBUG(("drop!"));
    }
    
}


void init(){
    api.getMyZRState(myState);
    game.dropSPS();//1st point to build the triangle
    imBlue=myState[1]>0;
    drawTriangle(imBlue);
}

void loop(){
    api.getMyZRState(myState);
    if(game.getNumSPSHeld()>0){//if you still have some SPSs
        drawTriangle(imBlue);
        DEBUG(("SPS draw triangle"));
    }
    else{
        switch(phase){
            case 0:
                game.getZone(ourZone);
                General::assign(theirZone,-ourZone[0],
                               -ourZone[1],-ourZone[2]);
                phase++;
            break;
            case 1:
            break;
        }
    }
    move.everySec(id, myState);
}
