#include "Menu.hpp"
   
Menu::Menu(TFT* display, Encoder* encoder, const char* title) {
    Menu::title = title;
    Menu::encoder = encoder;
    Menu::display = display;
}

void turn(encoder_actions action, void* o) { 
    Menu* menu = ((Menu*) o); 
    switch (action)
    {
    case RIGHT:
        (*menu).scrollUp();
        break; 
    case LEFT:
        (*menu).scrollDown();
        break; 
    case BUTTON:
        (*menu).select();
        break; 
    default:
        break;
    }
    (*menu).update();
}

void Menu::initialize() {
    display->drawHorizontalLine(lineheight, gap);  
    display->printTextCentered(title, lineheight-gap/2);   
    DisplaySize size = display->getSize();
    display->drawLine(size.width-gap*2, lineheight+gap, size.width-gap*2, size.height-gap);
 
    encoder->setCallback(turn, this);
}

void Menu::scrollDown() {
    currentSelected++;
    select();
}

void Menu::scrollUp() {
    currentSelected--;
    select();
}

void Menu::select() { 
    if(currentSelected < 0) currentSelected = lastItem-1;
    currentSelected %= lastItem; 
    for(int i=0; i<=lastItem; i++) { 
        MenuItem* item = &(items[i]); 
        item->selected = 0;
    }  
    items[currentSelected].selected = 1;
}

void Menu::addItem(int id, const char* label) {  
    MenuItem_t* newItems = new MenuItem_t[lastItem+1];
    if(lastItem > 0) {
        for(int i=0; i<lastItem; i++) {
            newItems[i] = items[i];
        }
    }
    lastItem++;
    MenuItem_t item = MenuItem();
    printf("size: %i", lastItem);
    printf(", item: %p", &item);
    item.itemName = String(label);
    item.id = id;
    item.selected = id == 0 ? 1 : 0;
    lastItem = id > lastItem ? id : lastItem;
    newItems[lastItem-1] = item;
    items = newItems;
    printf(", item Name: %s\n", (&newItems[lastItem-1])->itemName);
}

void Menu::update() {
    int offset = getSelectedItem(); 
    offset=floor(offset/5)*5;
    printf("%i .. %i\n", offset, getSelectedItem());
    MenuItem* newItemList [5] = {}; 
    printf("%i .. %i\n", offset, offset+5);
    for(int i=0; i<5; i++) {
        printf("%p .. ", &(items[i+offset]));
        if(i+offset > lastItem-1) continue;
        if(&(items[i+offset]) == nullptr) continue;
        MenuItem* item = &(items[i+offset]); 
        if(item->id == -1) continue;
        newItemList[i] = item;
    } 
    printf("\n");

    DisplaySize size = display->getSize();
    display->clear(gap, lineheight+gap/2, size.width-gap*2, size.height-gap);
    printf("%i .. %i\n", offset, offset+5);
        
    for(int i=0; i<5; i++) {  
        printf("%p .. ", newItemList[i]);
        if(newItemList[i] == nullptr) continue;
        MenuItem* item = newItemList[i];
        if(item->selected == 1) {
            display->addSelectedButton(item->itemName.c_str(), lineheight+gap/2+lineheight*i);
            continue;
        }
        display->addButton(item->itemName.c_str(), lineheight+gap/2+lineheight*i);
    }   
    printf("\n");
}

int Menu::getSelectedItem() {
    return currentSelected;
}
           