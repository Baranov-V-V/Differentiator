#pragma once

const int simplify_messages_begin_count = 6;
const int simplify_messages_end_count = 4;

const int before_differentiation_messages_count = 4;
const int after_differentiation_messages_count = 3;

//introduction
const char* intro_message_begin = "Advanced methods of differentiation";
const char* initials_message = "By Baranov Victor Vladimirovich";

const char* intro_message_description = "The derivative of a function of a real variable measures \
the sensitivity to change of the function value (output value)\
with respect to a change in its argument (input value). The process of finding a derivative is called differentiation. \
The reverse process is called antidifferentiation. The fundamental theorem of calculus relates antidifferentiation with integration.\
Differentiation and integration constitute the two fundamental operations in single-variable calculus. So it it is very important to\
take a derivative fast and correct";

const char* intro_message_end = "Let's take the derivative of the following function:";

//end of report
const char* outro_message = "The final derivative of the given function is:";

//used library
const char* literature_message = "Materials used in my report:";
const char* references_message1 = "Redkozubov's conspects and lections";
const char* references_message2 = "\"Collection of problems in mathematical analysis\" by Kudriavcev L.D.";
const char* references_thanks = "Special thanks to Vasenin Egor for helping me create this report in LaTex";

//differensiation message
const char* before_differentiation_messages[] = {
    "I guess the differential of:",
    "The Differential of the function:",
    "Taking derivative of:",
    "Obviously that this mathematical function after differentiation"
};

const char* after_differentiation_messages[] = {
    "Results in:",
    "Have value of:",
    "After long calculations we get:"
};

//simplification messages
const char* before_simplify_message = "Now simplify the following";

//before node dump
const char* simplify_messages_begin[] = {
    "Easy to notice that",
    "Even blind person sees that",
    "If your iq is more than 40 you'll get that",
    "No need to tell you that",
    "Nothing is easier that understanding that",
    "It's obvious to a 3rd grade that"
};
//after node dumb
const char* simplify_messages_end[] = {
    "Equals to",
    "Have the same value as",
    "Like",
    "Is 100\\% the same to the"
};
