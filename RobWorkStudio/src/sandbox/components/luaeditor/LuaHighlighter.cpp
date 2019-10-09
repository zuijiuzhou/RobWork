/*
 * LuaHighlighter.cpp
 *
 *  Created on: 23/10/2009
 *      Author: jimali
 */

#include "LuaHighlighter.hpp"

#include <QtGui>

LuaHighlighter::LuaHighlighter (QTextDocument* parent) :
    QSyntaxHighlighter (parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground (Qt::darkBlue);
    keywordFormat.setFontWeight (QFont::Bold);
    QStringList keywordPatterns;

    keywordPatterns << "\\bdo\\b"
                    << "\\bend\\b"
                    << "\\bwhile\\b"
                    << "\\brepeat\\b"
                    << "\\buntil\\b"
                    << "\\bif\\b"
                    << "\\belseif\\b"
                    << "\\belse\\b"
                    << "\\bfor\\b"
                    << "\\bin\\b"
                    << "\\bfunction\\b"
                    << "\\blocal\\b"
                    << "\\bnil\\b"
                    << "\\bfalse\\b"
                    << "\\btrue\\b"
                    << "\\band\\b"
                    << "\\bor\\b"
                    << "\\bnot\\b"
                    << "\\bthen\\b"
                    << "\\bbreak\\b";

    foreach (const QString& pattern, keywordPatterns)
    {
        rule.pattern = QRegExp (pattern);
        rule.format  = keywordFormat;
        highlightingRules.append (rule);
    }

    classFormat.setFontWeight (QFont::Bold);
    classFormat.setForeground (Qt::darkMagenta);
    rule.pattern = QRegExp ("\\bQ[A-Za-z]+\\b");
    rule.format  = classFormat;
    highlightingRules.append (rule);

    singleLineCommentFormat.setForeground (Qt::red);
    rule.pattern = QRegExp ("--(?!\\[)[^\n]*");
    rule.format  = singleLineCommentFormat;
    highlightingRules.append (rule);

    multiLineCommentFormat.setForeground (Qt::red);

    quotationFormat.setForeground (Qt::darkGreen);
    rule.pattern = QRegExp ("\".*\"");
    rule.format  = quotationFormat;
    highlightingRules.append (rule);

    functionFormat.setFontItalic (true);
    functionFormat.setForeground (Qt::blue);
    rule.pattern = QRegExp ("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format  = functionFormat;
    highlightingRules.append (rule);

    commentStartExpression = QRegExp ("--\\[");
    commentEndExpression   = QRegExp ("\\]");
}

void LuaHighlighter::highlightBlock (const QString& text)
{
    foreach (const HighlightingRule& rule, highlightingRules)
    {
        QRegExp expression (rule.pattern);
        int index = expression.indexIn (text);
        while (index >= 0) {
            int length = expression.matchedLength ();
            setFormat (index, length, rule.format);
            index = expression.indexIn (text, index + length);
        }
    }
    setCurrentBlockState (0);

    int startIndex = 0;
    if (previousBlockState () != 1)
        startIndex = commentStartExpression.indexIn (text);

    while (startIndex >= 0) {
        int endIndex = commentEndExpression.indexIn (text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState (1);
            commentLength = text.length () - startIndex;
        }
        else {
            commentLength =
                endIndex - startIndex + commentEndExpression.matchedLength ();
        }
        setFormat (startIndex, commentLength, multiLineCommentFormat);
        startIndex =
            commentStartExpression.indexIn (text, startIndex + commentLength);
    }
}
