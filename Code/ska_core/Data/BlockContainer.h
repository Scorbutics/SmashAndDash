#pragma once

//TODO Voué à disparaître au profit d'un évènement
namespace ska {
    class BlockContainer {
    public:
        BlockContainer() = default;
        virtual ~BlockContainer() = default;

        virtual unsigned int getBlockSize() const = 0;
        virtual bool isSameBlockId(const ska::Point<int>& centerPos, const ska::Point<int>& oldCenterPos, int) const = 0;
    };
}
