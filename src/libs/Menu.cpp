#include "Menu.hpp"
void resetMenuTitle(Menu *menu);

Menu::Menu(TFT* display, Encoder* encoder, const char* title) {
    Menu::title = title;
    Menu::encoder = encoder;
    Menu::display = display;
}

void turn(encoder_actions_t action, void* o) { 
    Menu* menu = ((Menu*) o); 
     
    printf("Action: ");
    switch (action) {
    case RIGHT:
        printf("RIGHT");
        menu->scrollUp();
        menu->update();
        break; 
    case LEFT:
        printf("LEFT");
        menu->scrollDown();
        menu->update();
        break; 
    case BUTTON:
        printf("SELECT");
        menu->select();
        break;  
    }
    printf("\n");
}

void Menu::begin() {
    display->drawHorizontalLine(lineheight, gap);  
    display->printTextCentered(title, lineheight-gap/2);   
    DisplaySize size = display->getSize();
    display->drawLine(size.width-gap*2, lineheight+gap, size.width-gap*2, size.height-gap);
 
    encoder->setCallback(turn, this);
}


void Menu::setCallback(void (*callback)(MenuItem_t* item)) {
    Menu::callback = callback;
}
void Menu::scrollDown() {
    currentSelected++; 
}

void Menu::scrollUp() {
    currentSelected--; 
}

void Menu::select() { 
    MenuItem_t* item = displayItems[2];
    callback(item);
}

void Menu::addItem(int id, const char* label) {  
    MenuItem_t* newItems = new MenuItem_t[lastItem+1];
    if(lastItem > 0) {
        for(int i=0; i<lastItem; i++) {
            newItems[i] = items[i];
        }
    }
    lastItem++;
    MenuItem_t item = MenuItem_t(); 
    item.itemName = String(label);
    item.id = id; 
    lastItem = id > lastItem ? id : lastItem;
    newItems[lastItem-1] = item;
    items = newItems;
}

void Menu::redraw() { 
    for(int i=0; i<5; i++) {   
        if(displayItems[i] == nullptr) continue;
        MenuItem_t* item = displayItems[i];
        if(i == 2) {
            display->addSelectedButton(item->itemName.c_str(), lineheight+gap/2+lineheight*i);
            continue;
        }
        display->addButton(item->itemName.c_str(), lineheight+gap/2+lineheight*i);
    }    
}

void Menu::changeOffset() {
    if(currentSelected < 0) currentSelected = lastItem-1;
    currentSelected %= lastItem; 
    int offset = getSelectedItem();   
    int conCurrentItem;
    for(int i=0; i<5; i++) { 
        conCurrentItem = i+offset; 
        conCurrentItem %= lastItem; 
        if(conCurrentItem > lastItem-1) continue;
        if(&(items[conCurrentItem]) == nullptr) continue;

        MenuItem_t* item = &(items[conCurrentItem]); 
        if(item->id == -1) continue;
        displayItems[i] = item;
    }
}

void Menu::update() {
    changeOffset();
    redraw();
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

    if(String(title).isEmpty()) {
        resetMenuTitle(this);
        return;
    }

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