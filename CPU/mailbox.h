#ifndef MAILBOX_H
#define MAILBOX_H

#include <queue>
#include <unordered_map>
#include <string>

// MailBox struct is intended to implement signal propagation from any stage to any stage. It is safe because only limited uint32_t data are transfered.

enum MailBoxNode {
    IF_Stage,
    ID_Stage,
    EXE_Stage,
    MEM_Stage,
    WB_Stage,
};

class MailBox {
private:
    // std::unordered_map<std::pair<MailBoxNode, MailBoxNode>, string>
public:
};


#endif  // MAILBOX_H
