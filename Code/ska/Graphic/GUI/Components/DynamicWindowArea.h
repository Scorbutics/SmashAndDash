// #pragma once
// #include <memory>
// #include "Window_Area.h"
// 
// class DialogMenu;
// 
// //TODO va disparaître au profit d'un Observer qui mettra à jour les données lors d'actions
// class DynamicWindowArea: public Window_Area {
// public:
// 	DynamicWindowArea(DialogMenu& parent);
// 	virtual ~DynamicWindowArea();
// 	virtual void refresh() = 0;
// 
// };
// 
// using DynamicWindowAreaPtr = std::unique_ptr<DynamicWindowArea>;