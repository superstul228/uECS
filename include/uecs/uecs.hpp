#include <set>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <cstdint>
#include <memory>
#include <typeindex>
#pragma once
namespace uecs{
	//ENTITY
	class Entity{	//basic entity
		uint32_t id;
	public:
		Entity(uint32_t id=0): id(id){}
		uint32_t GetId() const {	//id getter
			return id;
		};
		bool operator==(const Entity& other) const {	//we can equal entities
        	    	return id == other.id;
        	}
        
        	bool operator!=(const Entity& other) const {
            		return id != other.id;
        	}
	};
}

namespace std {	//soem hash func
    	template<>
    	struct hash<uecs::Entity> {
        	size_t operator()(const uecs::Entity& e) const {
            		return hash<uint32_t>()(e.GetId());
		}
	};
}

namespace uecs{
	//STORAGE INTERFACE
	struct StorageInterface{			//inteface for all component storages
		virtual ~StorageInterface() = default; 	//basic destructor
		virtual void RemoveEntity(Entity e) = 0;//remove entity from storage
		virtual bool Has(Entity e) = 0;		//check entity
		virtual void clear() = 0;		//clear storage
	};

	//STORAGE
	template <typename T>
	class Storage : public StorageInterface{		//storage realisation
		std::unordered_map<Entity, T> components;	//THE MADAFAKIN STORAGE
	public:
		T* Get(Entity e){				//get component value
			auto t = components.find(e);
			if (t != components.end()){
				return &t->second;
			}
			return nullptr;
		}
		void Clear(){					//clearer realisation
			components.clear();
		}
		void Add(Entity e, const T& component){		//add entity to storage
			components[e] = component;
		}
		void Remove(Entity e){				//remover func realisation
			components.erase(e);
		}
		void RemoveEntity(Entity e) override {		//remover func realisation#2
			Remove(e);
		}
		bool Has(Entity e) override {			//check existense of entity
			if (components.find(e) == components.end()){
				return 0;
			} else {
				return 1;
			}
		}
		void clear() override{				//clear storage
			components.clear();
		}
		auto& GetComponents(){	//components getter
			return components;
		}
		const auto& GetComponents() const {
			return components;
		}
	};

	//REGISTRY
	class Registry{	//holds all entities, ids, components and other
		std::set<uint32_t> FreeIds;
		uint32_t NextId = 1;
		std::unordered_map<std::type_index, std::unique_ptr<StorageInterface>> Storages;
		std::unordered_map<Entity, std::vector<std::type_index>> EntityComponents;
	public:

		//ENTITY CREATOR
		Entity Create(){
			uint32_t ret = 0;
			if (!FreeIds.empty()){
				ret = *FreeIds.begin();
				FreeIds.erase(ret);
			} else {
				ret = NextId;
				NextId++;
			}
			Entity e(ret);
			EntityComponents[e];
			return e;
		}

		//ENTITY REMOVER
		void Destroy(Entity e){	//entity remover
			for (auto& type : EntityComponents[e]){
				auto it = Storages.find(type);
				if (it != Storages.end()){
					it->second->RemoveEntity(e);
				}
			}
			EntityComponents.erase(e);
			FreeIds.insert(e.GetId());
		}

		//GET STORAGE FUNC
    		template<typename T>
    		Storage<T>& GetStorage() {
        		auto key = std::type_index(typeid(T));
        		auto it = Storages.find(key);
        		if (it == Storages.end()) {
				auto storage = std::make_unique<Storage<T>>();
				auto& ref = *storage;
				Storages[key] = std::move(storage);
            			return ref;
       			}
        		return static_cast<Storage<T>&>(*it->second);
    		}
    
		//STORAGE WRAPPERS
    		template<typename T>
    		void Add(Entity e, const T& component) {
        		GetStorage<T>().Add(e, component);
        		EntityComponents[e].push_back(std::type_index(typeid(T)));
    		}
    
    		template<typename T>
    		T* Get(Entity e) {
        		return GetStorage<T>().Get(e);
    		}
    
    		template<typename T>
    		bool Has(Entity e) {
        		return GetStorage<T>().Has(e);
    		}
    
    		template<typename T>
    		void Remove(Entity e) {
        		GetStorage<T>().Remove(e);
		}
		template<typename T>
		std::unordered_map<Entity, T> GetComponents(){
			GetStorage<T>().GetComponents();
		}
	};
}
