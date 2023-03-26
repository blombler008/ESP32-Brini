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
    if(currentSelected < 0) currentSelected = lastItem;
    currentSelected %= lastItem+1; 
    for(int i=0; i<=lastItem; i++) { 
        MenuItem* item = &(items[i]); 
        item->selected = 0;
    }  
    items[currentSelected].selected = 1;
}

void Menu::addItem(int id, const char* label) { 
    MenuItem_t* item = &items[id];
    item->itemName = String(label);
    item->id = id;
    item->selected = id == 0 ? 1 : 0;
    lastItem = id > lastItem ? id : lastItem;
}

void Menu::update() {
    int offset = getSelectedItem();
    if(offset >= 5) offset -= 5;
    if(offset < 5) offset = 0;
    for(int i=4; i>=0; i--) {
        MenuItem* item = &(items[i+offset]); 
        if(item->id == -1) continue;
        if(item->selected == 1) {
            display->addSelectedButton(item->itemName.c_str(), lineheight+gap/2+lineheight*i);
            continue;
        }
        display->addButton(item->itemName.c_str(), lineheight+gap/2+lineheight*i);
    }  
}

int Menu::getSelectedItem() {
    return currentSelected;
}
           