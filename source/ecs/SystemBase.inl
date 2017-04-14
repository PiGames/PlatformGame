template<class ComponentType>
inline componentWrapper_t SystemBase::AddComponent( entityID_t entity )
{
	size_t componentHashCode = typeid( ComponentType ).hash_code();

	if ( !this->isComponentRegistered( componentHashCode ) )
		this->registerComponent( componentHashCode );
	if ( this->isCurrentBlockOverloaded( componentHashCode ) )
		this->allocateNewBlock<ComponentType>();

	return this->addToBlock( entity, componentHashCode );
}

template<class ComponentType>
inline std::shared_ptr<std::vector<std::reference_wrapper<internal::componentBlock_t>>> SystemBase::ReserveComponentBlocks( size_t amount )
{
	size_t componentHashCode = typeid( ComponentType ).hash_code();
	auto blocks = std::make_shared<std::vector<std::reference_wrapper<internal::componentBlock_t>>>();

	if ( !this->isComponentRegistered( componentHashCode ) )
		this->registerComponent( componentHashCode );

	ECS_ASSERT( this->componentsBlocks.size() + amount <= MAX_COMPONENT_BLOCKS, "Reserving this amount of componentBlock_t will cause overflow" );
	for ( size_t i = 0; i < amount; i++ )
	{
		this->componentsBlocks.push_back( { componentHashCode } );
		this->componentsBlocks.back().ReserveComponents<ComponentType>( MAX_COMPONENT_BLOCK_SIZE );
	}

	blocks->reserve( amount );
	blocks->insert( blocks->end(), this->componentsBlocks.end() - amount, this->componentsBlocks.end() );

	return blocks;
}

template<class ComponentType>
inline componentWrapper_t SystemBase::GetComponent( entityID_t entity )
{
	if ( entity == UNASSIGNED_ENTITY_ID || !this->isEntityInSystem( entity ) )
		return componentWrapper_t();
	size_t componentHashCode = typeid( ComponentType ).hash_code();

	if ( !this->isComponentRegistered( componentHashCode ) )
		return componentWrapper_t();

	auto components = this->GetAllComponentsOfType<ComponentType>();

	for ( componentWrapper_t& component : *components)
		if ( component.ownerEntityID == entity )
			return component;

	return componentWrapper_t();
}

template<class ComponentType>
inline bool SystemBase::HasComponent( entityID_t entity )
{
	// Should this be better optimazed?
	return this->GetComponent<ComponentType>( entity ).ownerEntityID != UNASSIGNED_ENTITY_ID;
}

template<class ComponentType, typename ...Args>
inline void SystemBase::ForEach( std::function<void( SystemBase&, componentWrapper_t&, Args... )> func, Args&&... args )
{
	if ( !func )
		return;

	size_t componentHashCode = typeid( ComponentType ).hash_code();
	if ( !this->isComponentRegistered( componentHashCode ) )
		return;

	for ( auto it = this->componentsBlocks.begin(); it != this->componentsBlocks.end(); it++ )
		if ( it->hashCode == componentHashCode )
			for ( auto& component : it->data )
				if ( component.ownerEntityID != UNASSIGNED_ENTITY_ID )
				{
					func( *this, component, std::forward<Args>( args )... );
				}
}

template<class ComponentType, typename Lambda, typename ...Args>
inline void SystemBase::ForEachLambda( Lambda func, Args&&... args )
{
	std::function<void( SystemBase&, componentWrapper_t&, Args... )> function = func;
	this->ForEach<ComponentType>( function, args... );
}

template<class ComponentType>
inline std::shared_ptr<std::vector<std::reference_wrapper<componentWrapper_t>>> SystemBase::GetAllComponentsOfType()
{
	size_t componentHashCode = typeid( ComponentType ).hash_code();
	if ( !this->isComponentRegistered( componentHashCode ) )
		return nullptr;

	auto vec = std::make_shared<std::vector<std::reference_wrapper<componentWrapper_t>>>();

	for ( auto it = this->componentsBlocks.begin(); it != this->componentsBlocks.end(); it++ )
		if ( it->hashCode == componentHashCode )
		{
			vec->reserve( vec->size() + it->data.size() );
			vec->insert( vec->end(), it->data.begin(), it->data.end() );
		}

	return vec;
}

template<class ComponentType>
inline void SystemBase::allocateNewBlock()
{
	ECS_ASSERT( this->componentsBlocks.size() <= MAX_COMPONENT_BLOCKS, "Components blocks overflow" );

	this->componentsBlocks.emplace_back();
	this->componentsBlocks.back().hashCode = typeid( ComponentType ).hash_code();
	this->componentsBlocks.back().ReserveComponents<ComponentType>( MAX_COMPONENT_BLOCK_SIZE );
}
