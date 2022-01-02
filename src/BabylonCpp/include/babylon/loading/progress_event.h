#ifndef BABYLON_LOADING_PROGRESS_EVENT_H
#define BABYLON_LOADING_PROGRESS_EVENT_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

/**
 * @brief The ProgressEvent class represents events measuring progress of an underlying process.
 */
class BABYLON_SHARED_EXPORT ProgressEvent {

public:
  /**
   * @brief Creates a ProgressEvent event with the given parameters.
   * @param type represents the name of the type of the ProgressEvent
   * @param lengthComputable indicates if the total work to be done, and the amount of work already
   * done, by the underlying process is calculable. In other words, it tells if the progress is
   * measurable or not. It defaults to false.
   * @param loaded represents the amount of work already performed by the underlying process. The
   * ratio of work done can be calculated with the property and ProgressEvent.total. When
   * downloading a resource using HTTP, this only represent the part of the content itself, not
   * headers and other overhead. It defaults to 0.
   * @param total represents the total amount of work that the underlying process is in the progress
   * of performing. When downloading a resource using HTTP, this only represent the content itself,
   * not headers and other overhead. It defaults to 0.
   */
  ProgressEvent(const std::string& type, bool lengthComputable = false, size_t loaded = 0,
                size_t total = 0);
  ProgressEvent(const ProgressEvent& other);
  ProgressEvent(ProgressEvent&& other);
  ProgressEvent& operator=(const ProgressEvent& other);
  ProgressEvent& operator=(ProgressEvent&& other);
  ~ProgressEvent(); // = default

protected:
  /**
   * @brief Gets the name of the type of the ProgressEvent.
   */
  [[nodiscard]] std::string get_type() const;

  /**
   * @brief Gets the boolean flag indicating if the total work to be done, and the amount of work
   * already done, by the underlying process is calculable. In other words, it tells if the progress
   * is measurable or not.
   */
  [[nodiscard]] bool get_lengthComputable() const;

  /**
   * @brief Gets the amount of work already performed by the underlying process.
   * The ratio of work done can be calculated with the property and ProgressEvent.total.
   */
  [[nodiscard]] size_t get_loaded() const;

  /**
   * @brief Gets the total amount of work that the underlying process is in the progress of
   * performing.
   */
  [[nodiscard]] size_t get_total() const;

public:
  /**
   * The name of the type of the ProgressEvent.
   */
  ReadOnlyProperty<ProgressEvent, std::string> type;

  /**
   * The Boolean flag indicating if the total work to be done, and the amount of work already done,
   * by the underlying process is calculable. In other words, it tells if the progress is measurable
   * or not.
   */
  ReadOnlyProperty<ProgressEvent, bool> lengthComputable;

  /**
   * The amount of work already performed by the underlying process. The ratio of work done can be
   * calculated with the property and ProgressEvent.total.
   */
  ReadOnlyProperty<ProgressEvent, size_t> loaded;

  /**
   * The total amount of work that the underlying process is in the progress of performing.
   */
  ReadOnlyProperty<ProgressEvent, size_t> total;

private:
  std::string _type;
  bool _lengthComputable;
  size_t _loaded;
  size_t _total;

}; // end of class ProgressEvent

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_PROGRESS_EVENT_H
