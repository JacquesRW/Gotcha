#include <chrono>
#include <cstdint>

class Timer
{
    public:
        Timer(int mT, int bY, int bYS)
        {
            mainTime = static_cast<std::int64_t>(mT) * 1000;
            byoYomi = static_cast<std::int64_t>(bY) * 1000;
            byoYomiStones = static_cast<std::int64_t>(bYS);
            usingMainTime = mainTime > 0;
            remainingTime = usingMainTime ? mainTime : byoYomi;
            remainingStones = byoYomiStones;
        }

        Timer() {}

        std::int64_t alloc() const
        {
            if (usingMainTime)
                return ((byoYomi < 100) ? remainingTime : mainTime) / 25;
            else
                return remainingTime / (remainingStones + 2);
        }

        void start()
        {
            clockStart = std::chrono::high_resolution_clock::now();
        }

        std::int64_t elapsed()
        {
            const auto end = std::chrono::high_resolution_clock::now();
            const auto dur = end - clockStart;
            const auto durMs = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
            return static_cast<std::int64_t>(durMs.count());
        }

        void stop(const bool wasPass)
        {
            const auto ms = elapsed();

            remainingTime -= ms;
            remainingStones -= !wasPass;

            if ((usingMainTime && (remainingTime <= 0))
                || (!usingMainTime && (byoYomiStones == 0)))
            {
                if (!usingMainTime)
                    remainingTime = 0;
                usingMainTime = false;
                remainingTime += byoYomi;
                remainingStones = byoYomiStones;
            }
        };

        void reset()
        {
            usingMainTime = mainTime > 0;
            remainingStones = byoYomiStones;
        }

    private:
        bool usingMainTime = false;
        std::chrono::time_point<std::chrono::high_resolution_clock> clockStart;
        std::int64_t remainingTime{};
        std::int64_t remainingStones{};
        std::int64_t mainTime{};
        std::int64_t byoYomi{};
        std::int64_t byoYomiStones{};
};