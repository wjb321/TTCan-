do the test on the list:

some basic operations:
add list item2
add list item3

delete list item2


/*******************list and list address*******************/
item                              adress				    
TestList                          0x200000b4					
TestList->pxIndex                 0x200000bc					
TestList->xListEnd                0x200000bc					
ListItem1                         0x200000c8					
ListItem2                         0x200000dc					
ListItem3                         0x200000f0					

/***************add ListItem2 at the end of list***************/
item                             address				    
TestList->pxIndex                 0x200000c8					
TestList->xListEnd->pxNext        0x200000dc					
ListItem2->pxNext                 0x200000c8					
ListItem1->pxNext                 0x200000f0					
ListItem3->pxNext                 0x200000bc					
/*******************connect the crossline fron previous and next********************/
TestList->xListEnd->pxPrevious    0x200000f0					
ListItem2->pxPrevious             0x200000bc					
ListItem1->pxPrevious             0x200000dc					
ListItem3->pxPrevious             0x200000c8					
/************************end**************************/
