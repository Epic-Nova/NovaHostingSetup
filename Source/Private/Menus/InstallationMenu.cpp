/// @file InstallationMenu.cpp
/// @brief The installation progress menu.

#include "NovaCore.h"
#include "NovaMinimal.h"
#include "Utils/ScrollableLog.h"
#include "MainMenu.h"
#include "Menus/InputMenus.h"
#include "Core/NovaFileOperations.h"
#include "Helpers/GitInstallHelper/GitInstallHelper.h"


namespace Menus
{
    using namespace ftxui;
    using namespace Core;

    std::mutex InstallationMenu::progressMutex_;

    std::shared_ptr<InstallationMenu> InstallationMenu::Create()
    {
        NOVA_LOG_VERBOSE("Creating InstallationMenu instance", LogType::Debug);
        auto instance = std::make_shared<InstallationMenu>();
        NOVA_LOG_VERBOSE("InstallationMenu instance created successfully", LogType::Debug);
        return instance;
    }

    InstallationMenu::~InstallationMenu()
    {
        NOVA_LOG_VERBOSE("InstallationMenu destructor called", LogType::Debug);
        try {
            std::lock_guard<std::mutex> lock{progressMutex_};
            NOVA_LOG_VERBOSE("InstallationMenu destructor: mutex locked", LogType::Debug);
            steps_.clear();
            NOVA_LOG_VERBOSE("InstallationMenu destructor: steps cleared", LogType::Debug);
        } catch (const std::exception& e) {
            NOVA_LOG_VERBOSE(("InstallationMenu destructor exception: " + std::string(e.what())).c_str(), LogType::Error);
        }
        NOVA_LOG_VERBOSE("InstallationMenu destructor completed", LogType::Debug);
    }

    void InstallationMenu::SetComplete()
    {
        NOVA_LOG_VERBOSE("InstallationMenu::SetComplete() called", LogType::Debug);
        try {
            std::lock_guard<std::mutex> lock{progressMutex_};
            NOVA_LOG_VERBOSE("SetComplete: mutex locked", LogType::Debug);
            if (!isComplete_) {  // Prevent double completion
                isComplete_ = true;
                currentMessage_ = "All installations completed successfully!";
                NOVA_LOG("Installation process completed successfully!", LogType::Log);
                NOVA_LOG_VERBOSE("SetComplete: completion state set", LogType::Debug);
            } else {
                NOVA_LOG_VERBOSE("SetComplete: already completed, skipping", LogType::Debug);
            }
        } catch (const std::exception& e) {
            NOVA_LOG_VERBOSE(("SetComplete exception: " + std::string(e.what())).c_str(), LogType::Error);
        }
        NOVA_LOG_VERBOSE("InstallationMenu::SetComplete() finished", LogType::Debug);
    }

    void InstallationMenu::SetError(const std::string& errorMessage)
    {
        NOVA_LOG_VERBOSE("InstallationMenu::SetError() called", LogType::Debug);
        try {
            std::lock_guard<std::mutex> lock{progressMutex_};
            NOVA_LOG_VERBOSE("SetError: mutex locked", LogType::Debug);
            if (!hasError_) {  // Prevent double error setting
                hasError_ = true;
                errorMessage_ = errorMessage;
                
                if (currentStepIndex_ >= 0 && currentStepIndex_ < static_cast<int>(steps_.size())) {
                    steps_[currentStepIndex_].error = true;
                    steps_[currentStepIndex_].status = "Failed: " + errorMessage;
                    NOVA_LOG_VERBOSE("SetError: step error state updated", LogType::Debug);
                }
                
                NOVA_LOG(("Installation error: " + errorMessage).c_str(), LogType::Error);
                NOVA_LOG_VERBOSE("SetError: error state set", LogType::Debug);
            } else {
                NOVA_LOG_VERBOSE("SetError: already in error state, skipping", LogType::Debug);
            }
        } catch (const std::exception& e) {
            NOVA_LOG_VERBOSE(("SetError exception: " + std::string(e.what())).c_str(), LogType::Error);
        }
        NOVA_LOG_VERBOSE("InstallationMenu::SetError() finished", LogType::Debug);
    }

    void InstallationMenu::Show()
    {
        NOVA_LOG("Showing enhanced installation menu!", LogType::Log);

        auto start_time = std::chrono::steady_clock::now();
        bool show_particles = true;

        // Create custom button style function with proper centering
        auto makeStyledButton = [&](const std::string& label, std::function<void()> callback, Color bg, Color fg, Color bg_active, Color fg_active) {
            auto option = ButtonOption::Animated(bg, fg, bg_active, fg_active);
            option.transform = (std::function<Element(const EntryState&)>) [=](const EntryState& state) -> Element {
                std::string prefix = state.focused ? "▶ " : "  ";
                std::string suffix = state.focused ? " ◀" : "  ";
                return hbox({
                    text(prefix),
                    text(state.label) | ftxui::bold | size(WIDTH, GREATER_THAN, 40),
                    text(suffix),
                }) | center | size(HEIGHT, GREATER_THAN, 1);
            };
            return Button(label, callback, option);
        };

        // Buttons for confirmation dialog
        auto ConfirmButton = makeStyledButton(
            "✅ Confirm",
            [&]() {
                NOVA_LOG("Installation confirmed!", LogType::Log);
                startInstallation_ = true;
                GetScreen().ExitLoopClosure()();
            },
            Color::Black, Color::Green, Color::Green, Color::White
        );
        
        auto CancelButton = makeStyledButton(
            "❌ Cancel",
            [&]() {
                NOVA_LOG("Installation canceled!", LogType::Log);
                startInstallation_ = false;
                GetScreen().ExitLoopClosure()();
            },
            Color::Black, Color::Red, Color::Red, Color::White
        );
        
        // Additional installation option buttons
        auto DetectButton = makeStyledButton(
            "🔍 Detect Installation Automatically",
            [&]() {
                NOVA_LOG("Auto-detection of installation initiated!", LogType::Log);
                if (DetectInstallationAutomatically()) {
                    installationMethod_ = InstallationMethod::AutoDetect;
                    NOVA_LOG("Auto-detection successful!", LogType::Log);
                } else {
                    NOVA_LOG("Auto-detection failed, please select another option.", LogType::Warning);
                }
            },
            Color::Black, Color::Cyan, Color::Cyan, Color::White
        );

        auto GitButton = makeStyledButton(
            "📥 Download from Remote using Git",
            [&]() {
                NOVA_LOG("Git download selected!", LogType::Log);
                if (SetupGitDownload()) {
                    installationMethod_ = InstallationMethod::GitDownload;
                    NOVA_LOG("Git setup complete!", LogType::Log);
                } else {
                    NOVA_LOG("Git setup canceled or failed.", LogType::Warning);
                }
            },
            Color::Black, Color::Blue, Color::Blue, Color::White
        );

        auto ManualButton = makeStyledButton(
            "📁 Enter Path Manually",
            [&]() {
                NOVA_LOG("Manual path entry selected!", LogType::Log);
                if (SetupManualPath()) {
                    installationMethod_ = InstallationMethod::ManualPath;
                    NOVA_LOG("Manual path setup complete!", LogType::Log);
                } else {
                    NOVA_LOG("Manual path setup canceled.", LogType::Warning);
                }
            },
            Color::Black, Color::Yellow, Color::Yellow, Color::Black
        );


        auto menu = Container::Vertical({
            DetectButton,
            GitButton,
            ManualButton,
            Renderer([] { return text("") | center; }), // Vertical spacer
            ConfirmButton,
            CancelButton
        });

        auto centered_menu = Renderer(menu, [menu] {
            return hbox({
                text("") | flex,
                menu->Render(),
                text("") | flex
            });
        });

        auto component = Renderer(centered_menu, [=]() -> Element {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();

            // Create animated rainbow colors
            auto getRainbowColor = [&](int offset) -> Color {
                int hue = (elapsed / 100 + offset) % 360;
                if      (hue < 60)  return Color::Red;
                else if (hue < 120) return Color::Yellow;
                else if (hue < 180) return Color::Green;
                else if (hue < 240) return Color::Cyan;
                else if (hue < 300) return Color::Blue;
                else                return Color::Magenta;
            };

            // Animated title
            std::vector<std::string> ascii_title {
            "██╗███╗   ██╗███████╗████████╗ █████╗ ██╗     ██╗     ",
            "██║████╗  ██║██╔════╝╚══██╔══╝██╔══██╗██║     ██║     ",
            "██║██╔██╗ ██║███████╗   ██║   ███████║██║     ██║     ",
            "██║██║╚██╗██║╚════██║   ██║   ██╔══██║██║     ██║     ",
            "██║██║ ╚████║███████║   ██║   ██║  ██║███████╗███████╗",
            "╚═╝╚═╝  ╚═══╝╚══════╝   ╚═╝   ╚═╝  ╚═╝╚══════╝╚══════╝"
            };
            Elements title_lines;
            for (size_t i = 0; i < ascii_title.size(); ++i) {
                auto line_color = getRainbowColor(i * 40);
                title_lines.push_back(text(ascii_title[i]) | color(line_color) | center);
            }

            // Particle effect
            auto createParticles = [&]() -> Element {
                if (!show_particles) return text("");
                Elements rows;
                for (int y = 0; y < 3; ++y) {
                    Elements row;
                    for (int x = 0; x < 40; ++x) {
                        auto t = ((elapsed + x * 87 + y * 159) / 80) % 8;
                        std::vector<std::string> sparkle{"·", "⋆", "✧", "✦", "*", "˚", "❉", "❋"};
                        row.push_back(text(sparkle[t]) | color(getRainbowColor(x * 9 + y * 20)));
                    }
                    rows.push_back(hbox(row));
                }
                return vbox(rows) | center | dim;
            };

            // Status bar
            auto createStatusBar = [&]() -> Element {
                Elements bar;
                for (int i = 0; i < 36; ++i) {
                    int idx = (elapsed / 120 + i) % 8;
                    std::vector<std::string> bars{"▁", "▂", "▃", "▄", "▅", "▆", "▇", "█"};
                    bar.push_back(text(bars[idx]) | color(getRainbowColor(i * 10)));
                }
                return hbox(bar) | center;
            };

            return vbox({
                createParticles(),
                vbox(title_lines),
                text("") | center,
                text("OPTIONS MENU") | ftxui::bold | color(Color::White) | center,
                text("") | center,
                text("Please select an installation option.") | color(Color::Yellow) | center,
                text("") | center,
                centered_menu->Render(),
                text("") | center,
                createStatusBar(),
            })
            | size(WIDTH, EQUAL, 80)
            | size(HEIGHT, EQUAL, 40)
            | border
            | center;
        });

        // Catch event to toggle particle effect
        auto enhanced_component = CatchEvent(component, [&](Event event) {
            if (event == Event::Character('p') || event == Event::Character('P')) {
                show_particles = !show_particles;
                return true;
            }
            return false;
        });

        GetScreen().Loop(enhanced_component);
    }

    void InstallationMenu::Hide()
    {
        NOVA_LOG("Hiding installation menu!", LogType::Log);
        NOVA_LOG_VERBOSE("InstallationMenu::Hide() called", LogType::Debug);
        try {
            GetScreen().ExitLoopClosure()();
            NOVA_LOG_VERBOSE("Hide: ExitLoopClosure called", LogType::Debug);
        } catch (const std::exception& e) {
            NOVA_LOG_VERBOSE(("Hide exception: " + std::string(e.what())).c_str(), LogType::Error);
        }
        NOVA_LOG_VERBOSE("InstallationMenu::Hide() finished", LogType::Debug);
    }

    void InstallationMenu::AddProgressStep(const std::string& stepName)
    {
        std::lock_guard<std::mutex> lock{progressMutex_};
        steps_.push_back({stepName, "Waiting...", false, false, {}, -1});
        NOVA_LOG(("Added progress step: " + stepName).c_str(), LogType::Debug);
    }

    void InstallationMenu::AddSubStep(const std::string& parentStep, const std::string& subStep)
    {
        std::lock_guard<std::mutex> lock{progressMutex_};
        
        for (auto& step : steps_) {
            if (step.name == parentStep) {
                step.subSteps.push_back({subStep, "Waiting...", false, false});
                NOVA_LOG(("Added substep: " + subStep + " to " + parentStep).c_str(), LogType::Debug);
                break;
            }
        }
    }

    void InstallationMenu::UpdateProgress(const std::string& message)
    {
        std::lock_guard<std::mutex> lock{progressMutex_};
        currentMessage_ = message;
        
        // Update current step status if we have one
        if (currentStepIndex_ >= 0 && currentStepIndex_ < static_cast<int>(steps_.size())) {
            steps_[currentStepIndex_].status = message;
        }
    }

    void InstallationMenu::SetCurrentStep(const std::string& stepName)
    {
        std::lock_guard<std::mutex> lock{progressMutex_};
        
        // Find the step and set it as current
        for (size_t i = 0; i < steps_.size(); i++) {
            if (i >= steps_.size()) continue; // Bounds check
            if (steps_[i].name == stepName) {
                currentStepIndex_ = static_cast<int>(i);
                steps_[i].status = "In Progress...";
                NOVA_LOG(("Set current step: " + stepName).c_str(), LogType::Debug);
                break;
            }
        }
    }

    void InstallationMenu::SetCurrentSubStep(const std::string& parentStep, const std::string& subStep)
    {
        std::lock_guard<std::mutex> lock{progressMutex_};
        
        for (auto& step : steps_) {
            if (step.name == parentStep) {
                for (size_t i = 0; i < step.subSteps.size(); i++) {
                    if (i >= step.subSteps.size()) continue; // Bounds check
                    if (step.subSteps[i].name == subStep) {
                        step.currentSubStepIndex = static_cast<int>(i);
                        step.subSteps[i].status = "In Progress...";
                        NOVA_LOG(("Set current substep: " + subStep + " in " + parentStep).c_str(), LogType::Debug);
                        break;
                    }
                }
                break;
            }
        }
    }

    void InstallationMenu::CompleteCurrentStep()
    {
        std::lock_guard<std::mutex> lock{progressMutex_};
        
        if (currentStepIndex_ >= 0 && currentStepIndex_ < static_cast<int>(steps_.size())) {
            steps_[currentStepIndex_].completed = true;
            steps_[currentStepIndex_].status = "Completed";
            NOVA_LOG(("Completed step: " + steps_[currentStepIndex_].name).c_str(), LogType::Debug);
        }
    }

    void InstallationMenu::CompleteCurrentSubStep()
    {
        std::lock_guard<std::mutex> lock{progressMutex_};
        
        if (currentStepIndex_ >= 0 && currentStepIndex_ < static_cast<int>(steps_.size())) {
            auto& currentStep = steps_[currentStepIndex_];
            if (currentStep.currentSubStepIndex >= 0 && 
                currentStep.currentSubStepIndex < static_cast<int>(currentStep.subSteps.size())) {
                currentStep.subSteps[currentStep.currentSubStepIndex].completed = true;
                currentStep.subSteps[currentStep.currentSubStepIndex].status = "Completed";
                NOVA_LOG(("Completed substep: " + currentStep.subSteps[currentStep.currentSubStepIndex].name).c_str(), LogType::Debug);
            }
        }
    }

    void InstallationMenu::StartInstallation()
    {
        NOVA_LOG("Starting installation process!", LogType::Log);
        
        if (installationMethod_ == InstallationMethod::None) {
            NOVA_LOG("No installation method selected!", LogType::Error);
            SetError("No installation method selected, please choose one first.");
            return;
        }
        
        if (ExecuteInstallation()) {
            NOVA_LOG("Installation executed successfully!", LogType::Log);
        } else {
            NOVA_LOG("Installation failed!", LogType::Error);
        }
    }

    void InstallationMenu::ViewProgress()
    {
        // Empty implementation for now
    }

    std::function<void(const std::string&)> InstallationMenu::GetProgressCallback()
    {
        return [this](const std::string& message) {
            this->UpdateProgress(message);
        };
    }

    Element InstallationMenu::RenderProgressList()
    {
        NOVA_LOG_VERBOSE("RenderProgressList: starting", LogType::Debug);
        try {
            // Note: This method is called from Show() which already has the lock
            Elements stepElements;
            
            for (size_t i = 0; i < steps_.size(); ++i) {
                const auto& step = steps_[i];
                
                std::string prefix;
                Color statusColor = Color::White;
                
                if (step.error) {
                    prefix = "✗ ";
                    statusColor = Color::Red;
                } else if (step.completed) {
                    prefix = "✓ ";
                    statusColor = Color::Green;
                } else if (static_cast<int>(i) == currentStepIndex_) {
                    prefix = "⟳ ";
                    statusColor = Color::Yellow;
                } else {
                    prefix = "○ ";
                    statusColor = Color::GrayDark;
                }
                
                stepElements.push_back(
                    hbox({
                        text(prefix) | color(statusColor),
                        text(step.name) | color(statusColor),
                        text(" - ") | color(Color::GrayDark),
                        text(step.status) | color(statusColor)
                    })
                );

                // Add substeps if they exist
                for (size_t j = 0; j < step.subSteps.size(); ++j) {
                    const auto& subStep = step.subSteps[j];
                    
                    std::string subPrefix = "  ";
                    Color subStatusColor = Color::GrayDark;
                    
                    if (subStep.error) {
                        subPrefix += "✗ ";
                        subStatusColor = Color::Red;
                    } else if (subStep.completed) {
                        subPrefix += "✓ ";
                        subStatusColor = Color::Green;
                    } else if (static_cast<int>(i) == currentStepIndex_ && 
                              static_cast<int>(j) == step.currentSubStepIndex) {
                        subPrefix += "⟳ ";
                        subStatusColor = Color::Yellow;
                    } else {
                        subPrefix += "○ ";
                        subStatusColor = Color::GrayDark;
                    }
                    
                    stepElements.push_back(
                        hbox({
                            text(subPrefix) | color(subStatusColor),
                            text(subStep.name) | color(subStatusColor),
                            text(" - ") | color(Color::GrayDark),
                            text(subStep.status) | color(subStatusColor)
                        })
                    );
                }
            }
            
            NOVA_LOG_VERBOSE("RenderProgressList: completed successfully", LogType::Debug);
            return vbox(stepElements) | frame;
        } catch (const std::exception& e) {
            NOVA_LOG_VERBOSE(("RenderProgressList exception: " + std::string(e.what())).c_str(), LogType::Error);
            return text("Error rendering progress") | color(Color::Red);
        }
    }

    Element InstallationMenu::RenderCurrentStatus()
    {
        NOVA_LOG_VERBOSE("RenderCurrentStatus: starting", LogType::Debug);
        try {
            // Note: This method is called from Show() which already has the lock
            if (hasError_) {
                NOVA_LOG_VERBOSE("RenderCurrentStatus: showing error", LogType::Debug);
                return vbox({
                    text("Error occurred:") | color(Color::Red) | ftxui::bold,
                    text(errorMessage_) | color(Color::Red)
                });
            }
            
            if (isComplete_) {
                NOVA_LOG_VERBOSE("RenderCurrentStatus: showing completion", LogType::Debug);
                return text("Installation completed successfully!") | color(Color::Green) | ftxui::bold;
            }
            
            if (currentStepIndex_ >= static_cast<int>(steps_.size()) || currentStepIndex_ < 0) {
                NOVA_LOG_VERBOSE("RenderCurrentStatus: no current step", LogType::Debug);
                return vbox({
                    text("Current Status:") | color(Color::Yellow) | ftxui::bold,
                    text("No status to display.") | color(Color::White)
                });
            }
            
            NOVA_LOG_VERBOSE("RenderCurrentStatus: showing current message", LogType::Debug);
            return vbox({
                text("Current Status:") | color(Color::Yellow) | ftxui::bold,
                text(currentMessage_) | color(Color::White)
            });
        } catch (const std::exception& e) {
            NOVA_LOG_VERBOSE(("RenderCurrentStatus exception: " + std::string(e.what())).c_str(), LogType::Error);
            return text("Error rendering status") | color(Color::Red);
        }
    }
}
