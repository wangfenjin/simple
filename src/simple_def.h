/**
 * @file entry.h
 * @author Dylan
 * @brief
 * @version 0.1
 * @date 2021-07-15
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef SRC_ENTRY_H_
#define SRC_ENTRY_H_

enum QueryOption {
    kDefault,
    kSimple,
    kJiebaCutWithHMM,
    kJiebaCutWithoutHMM,
    kJiebaCutAll,
    kJiebaCutForSearch,
    kJiebaCutHMM,
    kJiebaCutSmall
};

#endif  // SRC_ENTRY_H_
