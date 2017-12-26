#include "kernel.h"

#include <iostream>
#include <algorithm>

namespace svm
{
    Kernel::Kernel(
                Scheduler scheduler,
                std::vector<Memory::ram_type> executables_paths
            )
        : board(),
          processes(),
          priorities(),
          scheduler(scheduler),
          _last_issued_process_id(0),
          _last_ram_position(0),
          _cycles_passed_after_preemption(0),
          _current_process_index(0)
    {
        std::for_each(
            executables_paths.begin(),
            executables_paths.end(),
            [&](Memory::ram_type &executable) {
                CreateProcess(executable);
            }
        );

        if (scheduler == FirstComeFirstServed) {
            board.pic.isr_0 = [&]() {
                // ToDo: Process the timer interrupt for the FCFS
            };

            board.pic.isr_3 = [&]() {
                // ToDo: Process the first software interrupt for the FCFS
                // Unload the current process

		std::cout << "Number of Processes left = " << processes.size() << std::endl;

		processes.pop_front();

		if (processes.empty()) {
			board.Stop();
		}
		else {
			board.cpu.registers = processes[0].registers;
			processes[0].state = Process::States::Running;
		}
            };
        } else if (scheduler == ShortestJob) {
            board.pic.isr_0 = [&]() {
                // ToDo: Process the timer interrupt for the Shortest
                //  Job scheduler
            };

            board.pic.isr_3 = [&]() {
                // ToDo: Process the first software interrupt for the Shortest
                //  Job scheduler

                // Unload the current process
		std::cout << "Number of Processes left = " << processes.size() << std::endl;

		processes.pop_front();

		if (processes.empty()) {
			board.Stop();
		}
		else {
			board.cpu.registers = processes[0].registers;
			processes[0].state = Process::States::Running;
		}
            };
        } else if (scheduler == RoundRobin) {
            board.pic.isr_0 = [&]() {
                // ToDo: Process the timer interrupt for the Round Robin
                //  scheduler
		std::cout << "States of processes: ";
		for (int i = 0; i < processes.size(); ++i) {
			std::cout << processes[i].state << " ";
		}
		std::cout << std::endl;
		_cycles_passed_after_preemption++;
		if (_cycles_passed_after_preemption > _MAX_CYCLES_BEFORE_PREEMPTION) {
			_cycles_passed_after_preemption = 0;
			processes[_current_process_index].registers = board.cpu.registers;
			processes[_current_process_index].state = Process::States::Ready;
			_current_process_index++;
			if (_current_process_index >= processes.size()) {
				_current_process_index = 0;
			}
			board.cpu.registers = processes[_current_process_index].registers;
			processes[_current_process_index].state = Process::States::Running;
		}
            };

            board.pic.isr_3 = [&]() {
                // ToDo: Process the first software interrupt for the
                //  Round Robin scheduler

                // Unload the current process
		std::cout << "Number of Processes left = " << processes.size() << std::endl;

		processes.erase(processes.begin() + _current_process_index);
		if (processes.empty()) {
			board.Stop();
		} else {
			if (_current_process_index >= processes.size()) {
				_current_process_index = 0;
			}
			board.cpu.registers = processes[_current_process_index].registers;
			processes[_current_process_index].state = Process::States::Running;
		}
            };
        } else if (scheduler == Priority) {
            board.pic.isr_0 = [&]() {
                // ToDo: Process the timer interrupt for the Priority Queue
                //  scheduler
		std::cout << "Priority of the current process = " << priorities.top().priority << "\t Cycles = " << _MAX_CYCLES_BEFORE_PREEMPTION << std::endl;

		_cycles_passed_after_preemption++;
		if (_cycles_passed_after_preemption > _MAX_CYCLES_BEFORE_PREEMPTION) {
			_cycles_passed_after_preemption = 0;

			Process temp = priorities.top();
			temp.registers = board.cpu.registers;
			temp.state = Process::States::Ready;
			priorities.pop();
			if (temp.priority > 0) {
				temp.priority--;
			}
			priorities.push(temp);
			if (priorities.empty()) {
				board.Stop();
			}
			else {
				Process t = priorities.top();
				board.cpu.registers = t.registers;
				t.state = Process::States::Running;
			}
		}
            };

            board.pic.isr_3 = [&]() {
                // ToDo: Process the first software interrupt for the Priority
                //  Queue scheduler

                // Unload the current process
		std::cout << "Number of Processes left = " << processes.size() << std::endl;

		if (board.cpu.registers.a == 1) {
			priorities.pop();
			if (priorities.empty()) {
				board.Stop();
			}
			else {
				Process t = priorities.top();
				board.cpu.registers = t.registers;
				priorities.pop();
				t.state = Process::States::Running;
				priorities.push(t);
			}
		} else if (board.cpu.registers.a == 2) {
			Process t = priorities.top();
			priorities.pop();
			t.priority = board.cpu.registers.b;
			priorities.push(t);
		}
            };
        }

        // ToDo

        // ---

        board.Start();
    }

    Kernel::~Kernel() { }

    void Kernel::CreateProcess(Memory::ram_type &executable)
    {
        std::copy(
            executable.begin(),
            executable.end(),
            board.memory.ram.begin() + _last_ram_position
        );

        Process process(
            _last_issued_process_id++,
            _last_ram_position,
            _last_ram_position + executable.size()
        );

        _last_ram_position +=
            executable.size();

        // ToDo: add the new process to an appropriate data structure
        //processes.push_back(process);

        // ToDo: process the data structure

	if (scheduler == Priority) {
		if (board.cpu.registers.a = 2) {
			process.priority = board.cpu.registers.b;
		}
		board.cpu.registers.a = 1;
		priorities.push(process);
	}
	else if (scheduler == ShortestJob) {
		processes.push_back(process);
		std::sort(processes.begin(), processes.end(), [](const Process &a, const Process &b) {
			return a.sequential_instruction_count > b.sequential_instruction_count;
		});
	}
	else {
		processes.push_back(process);
	}
    }
}
