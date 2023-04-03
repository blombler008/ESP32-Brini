#include "Menu.hpp"
void resetMenuTitle(Menu *menu);

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
        menu->scrollUp();
        break; 
    case LEFT:
        menu->scrollDown();
        break; 
    case BUTTON:
        menu->select();
        break; 
    default:
        break;
    }
    menu->update();
}

void Menu::begin() {
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
    // for(int i=0; i<=lastItem; i++) { 
    //     MenuItem* item = &(items[i]);  
    // }   
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
    item.itemName = String(label);
    item.id = id; 
    lastItem = id > lastItem ? id : lastItem;
    newItems[lastItem-1] = item;
    items = newItems;
}

void Menu::update() {
    int offset = getSelectedItem();  
    MenuItem* newItemList [5] = {};  
    int conCurrentItem;
    for(int i=0; i<5; i++) { 
        conCurrentItem = i+offset; 
        conCurrentItem %= lastItem; 
        if(conCurrentItem > lastItem-1) continue;
        if(&(items[conCurrentItem]) == nullptr) continue;

        MenuItem* item = &(items[conCurrentItem]); 
        if(item->id == -1) continue;
        newItemList[i] = item;
    }  
 
        
    for(int i=0; i<5; i++) {   
        if(newItemList[i] == nullptr) continue;
        MenuItem* item = newItemList[i];
        if(i == 2) {
            display->addSelectedButton(item->itemName.c_str(), lineheight+gap/2+lineheight*i);
            continue;
        }
        display->addButton(item->itemName.c_str(), lineheight+gap/2+lineheight*i);
    }    
}

int Menu::getSelectedItem() {
    return currentSelected;
}

int Menu::getGap() {
    return gap;
}

int Menu::getLineHeight() {
    return lineheight;
}

const char* Menu::getTitle() {
    return title;
}

const uint8_t * Menu::getTitleFont() {
    return fontTitle;
}

const uint8_t * Menu::getButtonFont() {
    return fontButton;
}

void Menu::setTitle(const char *title) {
    DisplaySize size = display->getSize();
    display->clear(1,1, size.width, lineheight-1);
    display->setFont(fontTitle);
    display->printTextCentered(title, lineheight-gap/2); 
    
    tkTitle.once(10, resetMenuTitle, this);
}

void resetMenuTitle(Menu *menu) {
    TFT* display = menu->display;
    int lineheight = menu->getLineHeight(), gap = menu->getGap();
    DisplaySize size = display->getSize();
    display->clear(1,1, size.width, lineheight-1);
    display->setFont(menu->getTitleFont());
    display->printTextCentered(menu->getTitle(), lineheight-gap/2);
}