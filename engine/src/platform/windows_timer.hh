#pragma once
#include "stdafx.hh"

#if defined(Q_PLATFORM_WINDOWS)
#include <windows.h>
#include <stdint.h>
#include <time.h>

// WINDOWS TIMER IMPLEMENTATION
class StepTimer {
    public:
        StepTimer() :
            m_elapsedTicks(0),
            m_totalTicks(0),
            m_leftOverTicks(0),
            m_frameCount(0),
            m_fps(0),
            m_framesThisSecond(0),
            m_qpcSecondCounter(0),
            m_isFixedTimeStep(false),
            m_targetElapsedTicks(0)
	    {
            QueryPerformanceFrequency(&m_qpcFrequency);
            QueryPerformanceCounter(&m_qpcLastTime);
	        m_qpcMaxDelta = m_qpcFrequency.QuadPart / 10;
	    }

        /// Accessors
        // Get elapsed time since the previous call
        uint64_t GetElapsedTicks() const { return m_elapsedTicks; }
        double GetElapsedSeconds() const { return TicksToSeconds(m_elapsedTicks); }

        // Get total time since beginning of program
        uint64_t GetTotalTicks() const { return m_totalTicks; }
        double GetTotalSeconds() const { return TicksToSeconds(m_totalTicks); }

        // Get current framerate
        uint32_t GetFPS() const { return m_fps; }

        // Get total number of updates since program start
        uint32_t GetFrameCount() const { return m_frameCount; }

        /// Mutators
        // Set whether to use fixed or variable timestep mode
        void SetFixedTimeStep(bool isFixed) { m_isFixedTimeStep = isFixed; }

        // Set how often to call update when in fixed timestep mode
        void SetTargetElapsedTicks(uint64_t target) { m_targetElapsedTicks = target; }
        void SetTargetElapsedSeconds(double target) { m_targetElapsedTicks = SecondsToTicks(target); }

        /// Static Members
        // Integer format represents time using 10_000_000 ticks per second
        static const uint64_t TicksPerSecond = 10000000;

        static double TicksToSeconds(uint64_t ticks) { return static_cast<double>(ticks) / TicksPerSecond; }
        static uint64_t SecondsToTicks(double seconds) { return static_cast<uint64_t>(seconds * TicksPerSecond); }

        // After an intentional timing discontinuity (like blocking for IO)
        // call this to avoid having the fixed timestep logic attempt a set
        // of catch-up Update calls
        void ResetElapsedTime() {
            QueryPerformanceCounter(&m_qpcLastTime);

            m_leftOverTicks = 0;
            m_fps = 0;
            m_framesThisSecond = 0;
            m_qpcSecondCounter = 0;
        }

        typedef void(*LPUPDATEFUNC) (void);

        // Update timer state, calling the specified update function 
        // the appropriate number of times
        void Tick(LPUPDATEFUNC update = nullptr) {
            LARGE_INTEGER currentTime;
            QueryPerformanceCounter(&currentTime);

            uint64_t timeDelta;
            timeDelta = static_cast<uint64_t>(currentTime.QuadPart - m_qpcLastTime.QuadPart);
            m_qpcLastTime = currentTime;

            m_qpcSecondCounter += timeDelta;

            // Clamp excessivley large time deltas (like after paused in debugger)
            if (timeDelta > m_qpcMaxDelta)
                timeDelta = m_qpcMaxDelta;

            // Now we have the elapsed num of ticks along with num of ticks/second
            // We use these to convert to num of elapsed microseconds
            // To guard against loss-of-precision, we convert to microseconds *before*
            // dividing by ticks/second
            // This cannot overflow due to previous clamp
            timeDelta *= TicksPerSecond;

            timeDelta /= static_cast<uint64_t>(m_qpcFrequency.QuadPart);

            uint32_t lastFrameCount = m_frameCount;

            if (m_isFixedTimeStep) {
                // If app is runniing close to target elapsed time (withing 1/4 of millisecond)
                // just clamp the clock to exactly match the target value. This prevents 
                // unecessary errors from accumulating over time. 
                if (static_cast<uint64_t>(abs(static_cast<int>(timeDelta - m_targetElapsedTicks))) < TicksPerSecond / 4000) {
                    timeDelta = m_targetElapsedTicks;
                }

                m_leftOverTicks += timeDelta;

                while (m_leftOverTicks >= m_targetElapsedTicks) {
                    m_elapsedTicks = m_targetElapsedTicks;
                    m_totalTicks += m_targetElapsedTicks;
                    m_leftOverTicks -= m_targetElapsedTicks;
                    m_frameCount++;

                    if (update) {
                        update();
                    }
                }
            } else {
                // Variable timestep
                m_elapsedTicks = timeDelta;
                m_totalTicks += timeDelta;
                m_leftOverTicks = 0;
                m_frameCount++;

                if (update)
                    update();
            }

            // Track current framerate
            if (m_frameCount != lastFrameCount)
                m_framesThisSecond++;

            if (m_qpcSecondCounter >= static_cast<uint64_t>(m_qpcFrequency.QuadPart)) {
                m_fps = m_framesThisSecond;
                m_framesThisSecond = 0;
                m_qpcSecondCounter %= static_cast<uint64_t>(m_qpcFrequency.QuadPart);
            }
        }




    private:

        LARGE_INTEGER m_qpcFrequency;
        LARGE_INTEGER m_qpcLastTime;

        uint64_t m_qpcMaxDelta;

        uint64_t m_elapsedTicks;
        uint64_t m_totalTicks;
        uint64_t m_leftOverTicks;

        // For tracking the framerate
        uint32_t m_frameCount;
        uint32_t m_fps;
        uint32_t m_framesThisSecond;
        uint32_t m_qpcSecondCounter;

        // For configuring the fixed timestep mode
        bool m_isFixedTimeStep;
        uint64_t m_targetElapsedTicks;
};

#endif // Q_PLATFORM_WINDOWS