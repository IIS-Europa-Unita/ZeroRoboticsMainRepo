class objSelection {
    private:
        static int itemChosen;
        static int objID = 0;
        static float objIDPos[3], objIDp1Pos[3]; //ObjIDPos = pos of objID; objIDp1Pos = pos of objID+1
        
        
        bool isNearer(float objPos) {
            if (!(game.hasItem(objID) != 2 || game.itemInZone(objID))) //Is obj free? 
                if (dist(myState, objPos) > dist(otState, objPos)) //Am i further than other?
                    return false;   //I am further
                else
                    return true;    //I am nearer
            else
                return false;   //Obj isn't free
        }        
    public:
        int getID(){
            itemChosen = -1; 
            
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
}
