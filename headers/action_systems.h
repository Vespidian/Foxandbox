#ifndef COLLISION_H_
#define COLLISION_H_


void FindCollisions();

void DropItem(ItemComponent *item, int qty, Vector2 pos);

extern DroppedItemComponent *droppedItems;

extern int numDroppedItems;

#endif