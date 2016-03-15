#include <foundation/uuid.hpp>
#include <foundation/logger.hpp>
#include <stdio.h>
#include "foo.hpp"

std::string Foo::hello() const
{
    return "hello";
}

std::string Foo::world() const
{
    return "world";
}

void Foo::hello_world(const Foo &foo)
{
    loginfo() << "[Testing] " << Jetpack::Foundation::UUID::Make()->stringRepresentation() << ": " << foo.hello() << ", " << foo.world();
}
